/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Definition of spline_l_curve class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <Eigen/Core>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/opt/any_objective_function.h"
#include "num_collect/opt/golden_section_search.h"
#include "num_collect/rbf/operators/gradient_operator.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"
#include "num_collect/regularization/concepts/regularized_solver.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/format_dense_vector.h"
#include "num_collect/util/vector.h"

namespace num_collect::regularization {

//! Tag of spline_l_curve.
constexpr auto spline_l_curve_tag =
    logging::log_tag_view("num_collect::regularization::spline_l_curve");

/*!
 * \brief Class to search optimal regularization parameter using L-curve with
 * interpolation.
 *
 * \tparam Solver Type of solvers.
 */
template <concepts::regularized_solver Solver>
class spline_l_curve : public logging::logging_mixin {
public:
    //! Type of solvers.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    //! Type of data.
    using data_type = typename solver_type::data_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] solver Solver.
     * \param[in] data Data.
     * \param[in] initial_solution Initial solution.
     */
    spline_l_curve(solver_type& solver, const data_type& data,
        const data_type& initial_solution)
        : logging::logging_mixin(spline_l_curve_tag),
          solver_(&solver),
          data_(&data),
          initial_solution_(&initial_solution) {
        this->configure_child_algorithm_logger_if_exists(optimizer_);
        this->configure_child_algorithm_logger_if_exists(*solver_);
        this->logger().set_iterative();
    }

    /*!
     * \brief Search the optimal regularization parameter.
     */
    void search() {
        using std::log10;
        using std::pow;

        const auto [min_param, max_param] = solver_->param_search_region();
        NUM_COLLECT_LOG_DEBUG(
            logger(), "Region of parameters: [{}, {}]", min_param, max_param);
        const scalar_type log_min_param = log10(min_param);
        const scalar_type log_max_param = log10(max_param);

        create_initial_sample_points(log_min_param, log_max_param);
        add_sample_points();
        remove_too_close_sample_points();
        interpolate_sample_points();
        search_opt_param();
    }

    /*!
     * \brief Get the optimal regularization parameter.
     *
     * \return Optimal regularization parameter.
     */
    [[nodiscard]] auto opt_param() const noexcept -> scalar_type {
        return opt_param_;
    }

    /*!
     * \brief Solve with the optimal regularization parameter.
     *
     * \tparam Solution Type of the solution.
     * \param[out] solution Solution.
     */
    void solve(data_type& solution) const {
        NUM_COLLECT_LOG_DEBUG(
            logger(), "Solve with an optimal parameter: {}", opt_param_);
        solver_->solve(opt_param_, solution);
    }

    /*!
     * \brief Set the number of sample points.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto num_sample_points(index_type value) -> spline_l_curve& {
        NUM_COLLECT_PRECONDITION(
            value >= 3, "Number of sample points must be 3 or more.");
        num_sample_points_ = value;
        return *this;
    }

    /*!
     * \brief Set the minimum distance between sample points.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto min_distance_between_points(scalar_type value) -> spline_l_curve& {
        NUM_COLLECT_PRECONDITION(value > static_cast<scalar_type>(0),
            "Minimum distance must be a positive value.");
        min_distance_between_points_ = value;
        return *this;
    }

    /*!
     * \brief Set the number of points for searching peaks of curvature.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto num_points_for_searching_curvature_peaks(index_type value)
        -> spline_l_curve& {
        NUM_COLLECT_PRECONDITION(
            value > 0, "Number of points must be a positive integer.");
        num_points_for_searching_curvature_peaks_ = value;
        return *this;
    }

    /*!
     * \brief Set the minimum curvature to consider as an optimal point in
     * L-curve.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto min_curvature_for_optimal_point(scalar_type value) -> spline_l_curve& {
        NUM_COLLECT_PRECONDITION(value > static_cast<scalar_type>(0),
            "Minimum curvature must be a positive value.");
        min_curvature_for_optimal_point_ = value;
        return *this;
    }

private:
    //! Struct of sample points.
    struct sample_point {
        //! Logarithm of the regularization parameter.
        scalar_type log_regularization_parameter;

        //! Logarithm of the norm of the residual.
        scalar_type log_residual_norm;

        //! Logarithm of the regularization term.
        scalar_type log_regularization_term;

        //! Distance from the previous point in the L-curve.
        scalar_type distance_from_previous_point;
    };

    /*!
     * \brief Create initial sample points.
     *
     * \param[in] log_min_param Logarithm of the minimum regularization
     * parameter.
     * \param[in] log_max_param Logarithm of the maximum regularization
     * parameter.
     */
    void create_initial_sample_points(
        scalar_type log_min_param, scalar_type log_max_param) {
        sample_points_.clear();
        sample_points_.push_back(compute_sample_point(log_min_param));
        sample_points_.push_back(compute_sample_point(
            // NOLINTNEXTLINE(*-magic-numbers)
            static_cast<scalar_type>(0.5) * (log_min_param + log_max_param)));
        sample_points_.push_back(compute_sample_point(log_max_param));
        calculate_distance(1);
        calculate_distance(2);
    }

    /*!
     * \brief Add sample points until the number of sample points reaches the
     * configured number.
     */
    void add_sample_points() {
        while (sample_points_.size() < num_sample_points_) {
            auto max_distance = static_cast<scalar_type>(0);
            index_type max_distance_index = 0;
            for (index_type i = 1; i < sample_points_.size(); ++i) {
                const auto& point = sample_points_[i];
                if (point.distance_from_previous_point > max_distance) {
                    max_distance = point.distance_from_previous_point;
                    max_distance_index = i;
                }
            }
            NUM_COLLECT_ASSERT(max_distance_index > 0);

            const scalar_type additional_log_param =
                // NOLINTNEXTLINE(*-magic-numbers)
                static_cast<scalar_type>(0.5) *
                (sample_points_[max_distance_index - 1]
                        .log_regularization_parameter +
                    sample_points_[max_distance_index]
                        .log_regularization_parameter);
            sample_points_.insert(sample_points_.begin() + max_distance_index,
                compute_sample_point(additional_log_param));

            calculate_distance(max_distance_index);
            calculate_distance(max_distance_index + 1);
        }
    }

    /*!
     * \brief Remove too close sample points.
     */
    void remove_too_close_sample_points() {
        for (auto iter = sample_points_.begin() + 1;
            iter != sample_points_.end();) {
            if (iter->distance_from_previous_point <
                min_distance_between_points_) {
                NUM_COLLECT_LOG_TRACE(logger(),
                    "Remove a too close sample point: reg_param = {}, distance "
                    "= {}",
                    pow(static_cast<scalar_type>(10),  // NOLINT
                        iter->log_regularization_parameter),
                    iter->distance_from_previous_point);
                iter = sample_points_.erase(iter);
                if (iter != sample_points_.end()) {
                    calculate_distance(
                        static_cast<index_type>(iter - sample_points_.begin()));
                }
            } else {
                ++iter;
            }
        }
    }

    /*!
     * \brief Interpolate sample points.
     */
    void interpolate_sample_points() {
        length_parameters_.resize(sample_points_.size());
        log_residual_norms_.resize(sample_points_.size());
        log_regularization_terms_.resize(sample_points_.size());
        auto length_parameter = static_cast<scalar_type>(0);
        {
            length_parameters_(0) = length_parameter;
            log_residual_norms_(0) = sample_points_[0].log_residual_norm;
            log_regularization_terms_(0) =
                sample_points_[0].log_regularization_term;
        }
        for (index_type i = 1; i < sample_points_.size(); ++i) {
            length_parameter += sample_points_[i].distance_from_previous_point;
            length_parameters_(i) = length_parameter;
            log_residual_norms_(i) = sample_points_[i].log_residual_norm;
            log_regularization_terms_(i) =
                sample_points_[i].log_regularization_term;
        }

        NUM_COLLECT_LOG_TRACE(logger(), "Length parameters: {}",
            util::format_dense_vector(length_parameters_));
        NUM_COLLECT_LOG_TRACE(logger(), "Log residual norms: {}",
            util::format_dense_vector(log_residual_norms_));
        NUM_COLLECT_LOG_TRACE(logger(), "Log regularization terms: {}",
            util::format_dense_vector(log_regularization_terms_));

        log_residual_norm_interpolator_.compute(
            length_parameters_, log_residual_norms_);
        log_regularization_term_interpolator_.compute(
            length_parameters_, log_regularization_terms_);
    }

    /*!
     * \brief Search the optimal regularization parameter using the
     * interpolators.
     */
    void search_opt_param() {
        NUM_COLLECT_PRECONDITION(
            length_parameters_.size() >= 4, "Too few sample points.");

        // At first, search for peaks.
        const scalar_type search_point_diff =
            (length_parameters_(length_parameters_.size() - 1) -
                length_parameters_(0)) /
            static_cast<scalar_type>(
                num_points_for_searching_curvature_peaks_ - 1);
        scalar_type max_peak_curvature =
            std::numeric_limits<scalar_type>::lowest();
        index_type max_peak_index = 0;
        scalar_type prev_prev_curvature =
            evaluate_curvature(static_cast<scalar_type>(0));
        scalar_type prev_curvature = evaluate_curvature(search_point_diff);
        for (index_type i = 2; i < num_points_for_searching_curvature_peaks_;
            ++i) {
            const scalar_type curvature = evaluate_curvature(
                static_cast<scalar_type>(i) * search_point_diff);
            if (prev_prev_curvature < prev_curvature &&
                prev_curvature > curvature) {
                NUM_COLLECT_LOG_TRACE(logger(), "Curvature peak at {}: {}",
                    static_cast<scalar_type>(i - 1) * search_point_diff,
                    prev_curvature);
                if (prev_curvature > max_peak_curvature) {
                    max_peak_curvature = prev_curvature;
                    max_peak_index = i - 1;
                }
            }
            prev_prev_curvature = prev_curvature;
            prev_curvature = curvature;
        }

        // To maximize the curvature, minimize the negated curvature.
        const auto objective_function =
            [this](scalar_type length_parameter) -> scalar_type {
            return -evaluate_curvature(length_parameter);
        };
        optimizer_.change_objective_function(objective_function);
        optimizer_.init(
            static_cast<scalar_type>(max_peak_index - 1) * search_point_diff,
            static_cast<scalar_type>(max_peak_index + 1) * search_point_diff);
        optimizer_.solve();

        const scalar_type max_curvature =
            -optimizer_.opt_value();  // Negate back.
        NUM_COLLECT_LOG_TRACE(logger(), "Maximum curvature: {}", max_curvature);
        if (max_curvature < min_curvature_for_optimal_point_) {
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to find an optimal regularization parameter "
                "due to small curvature: {}",
                max_curvature);
        }

        const scalar_type opt_length_param = optimizer_.opt_variable();
        NUM_COLLECT_LOG_TRACE(
            logger(), "Optimal length parameter: {}", opt_length_param);

        const scalar_type log_opt_param =
            convert_length_param_to_log_reg_param(opt_length_param);
        opt_param_ = pow(static_cast<scalar_type>(10),  // NOLINT
            log_opt_param);
        NUM_COLLECT_LOG_SUMMARY(logger(), "Selected parameter: {}", opt_param_);
    }

    /*!
     * \brief Compute a sample point.
     *
     * \param[in] log_regularization_parameter Logarithm of the regularization
     * parameter.
     * \return Sample point.
     */
    [[nodiscard]] auto compute_sample_point(
        scalar_type log_regularization_parameter) -> sample_point {
        using std::pow;
        const scalar_type regularization_parameter =
            pow(static_cast<scalar_type>(10),  // NOLINT
                log_regularization_parameter);

        solution_ = *initial_solution_;
        solver_->solve(regularization_parameter, solution_);

        const scalar_type residual_norm = solver_->residual_norm(solution_);
        const scalar_type regularization_term =
            solver_->regularization_term(solution_);

        NUM_COLLECT_LOG_DEBUG(logger(),
            "Sample point: reg_param = {}, residual_norm = {}, reg_term = {}",
            regularization_parameter, residual_norm, regularization_term);

        const scalar_type log_residual_norm = std::log10(residual_norm);
        const scalar_type log_regularization_term =
            std::log10(regularization_term);

        return sample_point{
            .log_regularization_parameter = log_regularization_parameter,
            .log_residual_norm = log_residual_norm,
            .log_regularization_term = log_regularization_term,
            .distance_from_previous_point = static_cast<scalar_type>(0)};
    }

    /*!
     * \brief Calculate the distance from the previous sample point.
     *
     * \param[in] sample_point_index Index of the sample point.
     */
    void calculate_distance(index_type sample_point_index) {
        using std::sqrt;

        NUM_COLLECT_ASSERT(sample_point_index > 0);
        NUM_COLLECT_ASSERT(sample_point_index < sample_points_.size());

        auto& current_point = sample_points_[sample_point_index];
        const auto& previous_point = sample_points_[sample_point_index - 1];

        const scalar_type log_residual_norm_diff =
            current_point.log_residual_norm - previous_point.log_residual_norm;
        const scalar_type log_regularization_term_diff =
            current_point.log_regularization_term -
            previous_point.log_regularization_term;
        const scalar_type distance =
            sqrt(log_residual_norm_diff * log_residual_norm_diff +
                log_regularization_term_diff * log_regularization_term_diff);
        current_point.distance_from_previous_point = distance;
    }

    /*!
     * \brief Evaluate the curvature of the L-curve at a point.
     *
     * \param[in] length_parameter Length parameter.
     * \return Curvature.
     */
    [[nodiscard]] auto evaluate_curvature(scalar_type length_parameter) const
        -> scalar_type {
        using rbf::operators::gradient_operator;
        using rbf::operators::laplacian_operator;
        using std::pow;

        const scalar_type log_residual_norm_derivative =
            log_residual_norm_interpolator_.evaluate(
                gradient_operator(length_parameter));
        const scalar_type log_regularization_term_derivative =
            log_regularization_term_interpolator_.evaluate(
                gradient_operator(length_parameter));
        const scalar_type log_residual_norm_second_derivative =
            log_residual_norm_interpolator_.evaluate(
                laplacian_operator(length_parameter));
        const scalar_type log_regularization_term_second_derivative =
            log_regularization_term_interpolator_.evaluate(
                laplacian_operator(length_parameter));

        const scalar_type curvature =
            (log_residual_norm_derivative *
                    log_regularization_term_second_derivative -
                log_regularization_term_derivative *
                    log_residual_norm_second_derivative) /
            pow(log_residual_norm_derivative * log_residual_norm_derivative +
                    log_regularization_term_derivative *
                        log_regularization_term_derivative,
                // NOLINTNEXTLINE(*-magic-numbers)
                static_cast<scalar_type>(1.5));

        NUM_COLLECT_LOG_TRACE(logger(), "Curvature at length parameter {}: {}",
            length_parameter, curvature);

        return curvature;
    }

    /*!
     * \brief Convert a length parameter to a logarithm of a regularization
     * parameter using the linear interpolation.
     *
     * \param[in] length_parameter Length parameter.
     * \return Logarithm of the regularization parameter.
     */
    [[nodiscard]] auto convert_length_param_to_log_reg_param(
        scalar_type length_parameter) const -> scalar_type {
        NUM_COLLECT_PRECONDITION(length_parameter >= length_parameters_(0),
            "Length parameter is too small.");
        NUM_COLLECT_PRECONDITION(length_parameter <=
                length_parameters_(length_parameters_.size() - 1),
            "Length parameter is too large.");

        const auto right_iter = std::lower_bound(length_parameters_.data() + 1,
            length_parameters_.data() + length_parameters_.size(),
            length_parameter);
        const index_type right_index =
            static_cast<index_type>(right_iter - length_parameters_.data());
        const index_type left_index = right_index - 1;
        NUM_COLLECT_ASSERT(left_index >= 0);

        const scalar_type rate =
            (length_parameter - length_parameters_(left_index)) /
            (length_parameters_(right_index) - length_parameters_(left_index));
        const scalar_type log_reg_param =
            sample_points_[left_index].log_regularization_parameter +
            rate *
                (sample_points_[right_index].log_regularization_parameter -
                    sample_points_[left_index].log_regularization_parameter);
        return log_reg_param;
    }

    //! Solver.
    solver_type* solver_;

    //! Data.
    const data_type* data_;

    //! Initial solution.
    const data_type* initial_solution_;

    //! Optimizer.
    opt::golden_section_search<
        opt::any_objective_function<scalar_type(scalar_type)>>
        optimizer_{};

    //! Interpolator of the function from length parameters to logarithm of the norm of the residual.
    rbf::global_rbf_polynomial_interpolator<scalar_type(scalar_type)>
        log_residual_norm_interpolator_{};

    //! Interpolator of the function from length parameters to logarithm of the regularization term.
    rbf::global_rbf_polynomial_interpolator<scalar_type(scalar_type)>
        log_regularization_term_interpolator_{};

    //! Length parameters.
    Eigen::VectorX<scalar_type> length_parameters_{};

    //! Logarithms of the norm of the residual.
    Eigen::VectorX<scalar_type> log_residual_norms_{};

    //! Logarithms of the regularization term.
    Eigen::VectorX<scalar_type> log_regularization_terms_{};

    //! Sample points.
    util::vector<sample_point> sample_points_{};

    //! Optimal regularization parameter.
    scalar_type opt_param_{};

    //! Buffer of the solution.
    data_type solution_{};

    //! Default number of sample points.
    static constexpr index_type default_num_sample_points = 30;

    //! Number of sample points. (Minimum value is 3.)
    index_type num_sample_points_{default_num_sample_points};

    //! Default minimum distance between sample points.
    static constexpr auto default_min_distance_between_points =
        static_cast<scalar_type>(0.1);

    //! Minimum distance between sample points.
    scalar_type min_distance_between_points_{
        default_min_distance_between_points};

    //! Default number of points for searching peaks of curvature.
    static constexpr index_type
        default_num_points_for_searching_curvature_peaks = 50;

    //! Number of points for searching peaks of curvature.
    index_type num_points_for_searching_curvature_peaks_{
        default_num_points_for_searching_curvature_peaks};

    //! Default minimum curvature to consider as an optimal point in L-curve.
    static constexpr auto default_min_curvature_for_optimal_point =
        static_cast<scalar_type>(0.5);

    //! Minimum curvature to consider as an optimal point in L-curve.
    scalar_type min_curvature_for_optimal_point_{
        default_min_curvature_for_optimal_point};
};

}  // namespace num_collect::regularization
