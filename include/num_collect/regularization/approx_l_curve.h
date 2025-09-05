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
 * \brief Definition of approx_l_curve class.
 */
#pragma once

#include <cmath>
#include <limits>
#include <optional>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/regularization/concepts/regularized_solver.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"

namespace num_collect::regularization {

//! Tag of approx_l_curve.
constexpr auto approx_l_curve_tag =
    logging::log_tag_view("num_collect::regularization::approx_l_curve");

/*!
 * \brief Class to search optimal regularization parameter using L-curve with
 * approximation.
 *
 * \tparam Solver Type of solvers.
 */
template <concepts::regularized_solver Solver>
class approx_l_curve : public logging::logging_mixin {
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
     * \param[in] initial_solution Initial solution.
     */
    approx_l_curve(solver_type& solver, const data_type& initial_solution)
        : logging::logging_mixin(approx_l_curve_tag),
          solver_(&solver),
          initial_solution_(&initial_solution) {
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
        find_opt_param_with_largest_approx_curvature();
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
    auto num_sample_points(index_type value) -> approx_l_curve& {
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
    auto min_distance_between_points(scalar_type value) -> approx_l_curve& {
        NUM_COLLECT_PRECONDITION(
            value > 0, "Minimum distance must be positive.");
        min_distance_between_points_ = value;
        return *this;
    }

    /*!
     * \brief Set the minimum curvature to consider as an optimal point in
     * L-curve.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto min_curvature_for_optimal_point(scalar_type value) -> approx_l_curve& {
        NUM_COLLECT_PRECONDITION(
            value > 0, "Minimum curvature must be positive.");
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

        //! Direction of the line from the previous point to this point in radians from x-axis.
        scalar_type direction_from_previous_point;
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
        calculate_distance_and_direction(1);
        calculate_distance_and_direction(2);
    }

    /*!
     * \brief Add sample points until the number of sample points reaches the
     * configured number.
     */
    void add_sample_points() {
        while (sample_points_.size() < num_sample_points_) {
            const auto largest_direction_change_index_result =
                find_largest_direction_change_index();
            if (largest_direction_change_index_result.has_value()) {
                divide_line(largest_direction_change_index_result.value());
            }

            if (sample_points_.size() >= num_sample_points_) {
                break;
            }

            const auto largest_distance_index_result =
                find_largest_distance_index();
            if (largest_distance_index_result.has_value()) {
                divide_line(largest_distance_index_result.value());
            }

            if (!largest_direction_change_index_result.has_value() &&
                !largest_distance_index_result.has_value()) {
                // No more division is possible.
                break;
            }
        }
    }

    /*!
     * \brief Find the regularization parameter with the largest approximate
     * curvature.
     */
    void find_opt_param_with_largest_approx_curvature() {
        using std::pow;
        constexpr auto log_base = static_cast<scalar_type>(10);

        scalar_type max_curvature = std::numeric_limits<scalar_type>::lowest();
        index_type max_curvature_index = 0;
        for (index_type i = 1; i < sample_points_.size() - 1; ++i) {
            const scalar_type direction_change =
                sample_points_[i + 1].direction_from_previous_point -
                sample_points_[i].direction_from_previous_point;
            const scalar_type average_distance = static_cast<scalar_type>(0.5) *
                (sample_points_[i + 1].distance_from_previous_point +
                    sample_points_[i].distance_from_previous_point);
            const scalar_type curvature = direction_change / average_distance;
            NUM_COLLECT_LOG_TRACE(this->logger(),
                "Curvature approximation: reg_param = {}, curvature = {}",
                pow(log_base, sample_points_[i].log_regularization_parameter),
                curvature);
            if (curvature > max_curvature) {
                max_curvature = curvature;
                max_curvature_index = i;
            }
        }
        if (max_curvature < min_curvature_for_optimal_point_) {
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to find an optimal regularization parameter "
                "due to small curvature: {}",
                max_curvature);
        }

        opt_param_ = pow(log_base,
            sample_points_[max_curvature_index].log_regularization_parameter);
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
            .distance_from_previous_point = static_cast<scalar_type>(0),
            .direction_from_previous_point = static_cast<scalar_type>(0)};
    }

    /*!
     * \brief Calculate the distance and direction from the previous sample
     * point.
     *
     * \param[in] sample_point_index Index of the sample point.
     */
    void calculate_distance_and_direction(index_type sample_point_index) {
        using std::atan2;
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
        if (distance < std::numeric_limits<scalar_type>::epsilon()) {
            current_point.distance_from_previous_point = distance;
            current_point.direction_from_previous_point =
                previous_point.direction_from_previous_point;
            return;
        }
        const scalar_type direction =
            atan2(log_regularization_term_diff, log_residual_norm_diff);

        current_point.distance_from_previous_point = distance;
        current_point.direction_from_previous_point = direction;
    }

    /*!
     * \brief Find the index with the largest direction change.
     *
     * \return Index with the largest direction change. Null if further
     * division of lines in the L-curve should not be done.
     */
    [[nodiscard]] auto find_largest_direction_change_index() const
        -> std::optional<index_type> {
        index_type max_direction_change_index = 0;
        scalar_type max_direction_change =
            std::numeric_limits<scalar_type>::lowest();
        for (index_type i = 1; i < sample_points_.size(); ++i) {
            if (sample_points_[i].distance_from_previous_point <
                min_distance_between_points_) {
                // Prevent dividing too small ranges.
                continue;
            }
            // Take into account only positive changes to search for positive
            // curvatures.
            const scalar_type direction_change =
                sample_points_[i].direction_from_previous_point -
                sample_points_[i - 1].direction_from_previous_point;
            if (direction_change > max_direction_change) {
                max_direction_change = direction_change;
                max_direction_change_index = i;
            }
        }
        if (max_direction_change_index == 0) {
            NUM_COLLECT_LOG_WARNING(this->logger(),
                "Failed to find the next regularization parameter to try. "
                "Perhaps the number of sample points is too large.");
            return std::nullopt;
        }

        return max_direction_change_index;
    }

    /*!
     * \brief Find the index with the largest distance.
     *
     * \return Index with the largest distance. Null if further
     * division of lines in the L-curve should not be done.
     */
    [[nodiscard]] auto find_largest_distance_index() const
        -> std::optional<index_type> {
        index_type max_distance_index = 0;
        scalar_type max_distance = std::numeric_limits<scalar_type>::lowest();
        for (index_type i = 1; i < sample_points_.size(); ++i) {
            if (sample_points_[i].distance_from_previous_point <
                min_distance_between_points_) {
                // Prevent dividing too small ranges.
                continue;
            }
            if (sample_points_[i].distance_from_previous_point > max_distance) {
                max_distance = sample_points_[i].distance_from_previous_point;
                max_distance_index = i;
            }
        }
        if (max_distance_index == 0) {
            NUM_COLLECT_LOG_WARNING(this->logger(),
                "Failed to find the next regularization parameter to try. "
                "Perhaps the number of sample points is too large.");
            return std::nullopt;
        }

        return max_distance_index;
    }

    /*!
     * \brief Divide a line in the L-curve.
     *
     * \param[in] index Index of the sample point after the divided line.
     */
    void divide_line(index_type index) {
        NUM_COLLECT_ASSERT(index > 0);
        NUM_COLLECT_ASSERT(index < sample_points_.size());
        NUM_COLLECT_ASSERT(sample_points_[index].distance_from_previous_point >=
            min_distance_between_points_);

        const scalar_type next_log_param =
            // NOLINTNEXTLINE(*-magic-numbers)
            static_cast<scalar_type>(0.5) *
            (sample_points_[index - 1].log_regularization_parameter +
                sample_points_[index].log_regularization_parameter);
        sample_points_.insert(sample_points_.begin() + index,
            compute_sample_point(next_log_param));
        calculate_distance_and_direction(index);
        calculate_distance_and_direction(index + 1);
    }

    //! Solver.
    solver_type* solver_;

    //! Initial solution.
    const data_type* initial_solution_;

    //! Sample points.
    util::vector<sample_point> sample_points_{};

    //! Optimal regularization parameter.
    scalar_type opt_param_{};

    //! Buffer of the solution.
    data_type solution_{};

    //! Default number of sample points.
    static constexpr index_type default_num_sample_points = 20;

    //! Number of sample points. (Minimum value is 3.)
    index_type num_sample_points_{default_num_sample_points};

    //! Default minimum distance between sample points.
    static constexpr auto default_min_distance_between_points =
        static_cast<scalar_type>(0.1);

    //! Minimum distance between sample points.
    scalar_type min_distance_between_points_{
        default_min_distance_between_points};

    //! Default minimum curvature to consider as an optimal point in L-curve.
    static constexpr auto default_min_curvature_for_optimal_point =
        static_cast<scalar_type>(0.1);

    //! Minimum curvature to consider as an optimal point in L-curve.
    scalar_type min_curvature_for_optimal_point_{
        default_min_curvature_for_optimal_point};
};

}  // namespace num_collect::regularization
