/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of gaussian_process_optimizer class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/get_size.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/isfinite.h"
#include "num_collect/base/norm.h"
#include "num_collect/base/precondition.h"
#include "num_collect/constants/pi.h"  // IWYU pragma: keep
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/opt/any_objective_function.h"
#include "num_collect/opt/concepts/objective_function.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/rbf/gaussian_process_interpolator.h"
#include "num_collect/util/assert.h"

namespace num_collect::opt {

//! Tag of gaussian_process_optimizer.
constexpr auto gaussian_process_optimizer_tag =
    logging::log_tag_view("num_collect::opt::gaussian_process_optimizer");

/*!
 * \brief Class of Gaussian process optimization
 * \cite Srinivas2010, \cite Brochu2010.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::objective_function ObjectiveFunction>
class gaussian_process_optimizer
    : public optimizer_base<gaussian_process_optimizer<ObjectiveFunction>> {
public:
    //! This class.
    using this_type = gaussian_process_optimizer<ObjectiveFunction>;

    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit gaussian_process_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<gaussian_process_optimizer<ObjectiveFunction>>(
              gaussian_process_optimizer_tag),
          obj_fun_(obj_fun) {
        this->configure_child_algorithm_logger_if_exists(interpolator_);
        this->configure_child_algorithm_logger_if_exists(
            lower_bound_optimizer_);
        lower_bound_optimizer_.max_evaluations(
            default_max_lower_bound_evaluations);
    }

    /*!
     * \brief Change the objective function.
     *
     * \param[in] obj_fun Objective function.
     */
    void change_objective_function(const objective_function_type& obj_fun) {
        obj_fun_ = obj_fun;
    }

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        if constexpr (base::concepts::real_scalar_dense_vector<variable_type>) {
            NUM_COLLECT_PRECONDITION(lower.size() == upper.size(),
                this->logger(),
                "Lower and upper limits must have the same size.");
        }
        lower_ = lower;
        upper_ = upper;
        dim_ = get_size(lower_);

        // First sample.
        obj_fun_.evaluate_on(upper_);
        opt_variable_ = upper_;
        opt_value_ = correct_value_if_needed(obj_fun_.value());
        variables_.resize(1);
        values_.resize(1);
        variables_[0] = opt_variable_;
        values_[0] = opt_value_;

        iterations_ = 0;
        evaluations_ = 1;

        // Second sample.
        evaluate_on(lower_);
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        NUM_COLLECT_ASSERT(!variables_.empty());
        NUM_COLLECT_ASSERT(
            variables_.size() == static_cast<std::size_t>(values_.size()));

        interpolator_.optimize_length_parameter_scale(variables_, values_);
        interpolator_.compute(variables_, values_);

        try_find_and_evaluate_using_lower_bound();

        ++iterations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return evaluations() >= max_evaluations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::configure_iteration_logger
     */
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<index_type>(
            "Eval.", &this_type::evaluations);
        iteration_logger.template append<value_type>(
            "Value", &this_type::opt_value);
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return opt_variable_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return opt_value_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterations
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return evaluations_;
    }

    /*!
     * \brief Set the maximum number of function evaluations.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_evaluations(index_type value) -> gaussian_process_optimizer& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Maximum number of function evaluations must be a positive "
            "integer.");
        max_evaluations_ = value;
        return *this;
    }

    /*!
     * \brief Set the maximum number of evaluations of lower bounds.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_lower_bound_evaluations(index_type value)
        -> gaussian_process_optimizer& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Maximum number of evaluations of lower bounds must be a positive "
            "integer.");
        lower_bound_optimizer_.max_evaluations(value);
        return *this;
    }

    /*!
     * \brief Set the coefficient of the threshold of distances between sample
     * points.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto distance_threshold_coeff(double value) -> gaussian_process_optimizer& {
        NUM_COLLECT_PRECONDITION(value > 0.0, this->logger(),
            "Coefficient of the threshold of distances between sample points "
            "must be positive.");
        distance_threshold_coeff_ = value;
        return *this;
    }

private:
    /*!
     * \brief Type of interpolator.
     */
    using interpolator_type =
        rbf::gaussian_process_interpolator<value_type(variable_type)>;

    /*!
     * \brief Evaluate function value.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(const variable_type& variable) {
        obj_fun_.evaluate_on(variable);
        const value_type value = correct_value_if_needed(obj_fun_.value());
        if (value < opt_value_) {
            opt_variable_ = variable;
            opt_value_ = value;
        }
        variables_.push_back(variable);
        values_.conservativeResize(values_.size() + 1);
        values_(values_.size() - 1) = value;
        ++evaluations_;
    }

    /*!
     * \brief Correct function values if needed.
     *
     * \param[in] value Function value.
     * \return Corrected value.
     */
    [[nodiscard]] static auto correct_value_if_needed(value_type value) noexcept
        -> value_type {
        constexpr auto safe_limit =
            std::numeric_limits<value_type>::max() * 1e-2;
        if (!isfinite(value) || value > safe_limit) {
            return safe_limit;
        }
        return value;
    }

    /*!
     * \brief Try to find a sample point using lower bounds and evaluate a
     * function value for the sample point.
     */
    void try_find_and_evaluate_using_lower_bound() {
        const auto lower_bound_function =
            [this](const variable_type& variable) -> value_type {
            using std::log;
            using std::sqrt;
            using std::max;
            using std::pow;

            const auto [mean, variance] =
                interpolator_.evaluate_mean_and_variance_on(variable);
            const auto variance_coeff = static_cast<value_type>(2) *
                log(pow(static_cast<value_type>(evaluations_),
                        static_cast<value_type>(0.5) *
                                static_cast<value_type>(dim_) +
                            static_cast<value_type>(2)) *
                    constants::pi<value_type> * constants::pi<value_type> /
                    static_cast<value_type>(0.3));
            return mean -
                sqrt(
                    max(variance_coeff * variance, static_cast<value_type>(0)));
        };
        lower_bound_optimizer_.change_objective_function(lower_bound_function);

        lower_bound_optimizer_.init(lower_, upper_);
        lower_bound_optimizer_.solve();

        using distance_type = std::decay_t<decltype(norm(
            lower_bound_optimizer_.opt_variable() - variables_.front()))>;
        distance_type min_distance = std::numeric_limits<distance_type>::max();
        for (const auto& variable : variables_) {
            min_distance = std::min(min_distance,
                norm(lower_bound_optimizer_.opt_variable() - variable));
        }
        const distance_type distance_threshold =
            static_cast<distance_type>(distance_threshold_coeff_) *
            norm(upper_ - lower_) /
            static_cast<distance_type>(max_evaluations_);
        if (min_distance >= distance_threshold) {
            evaluate_on(lower_bound_optimizer_.opt_variable());
            return;
        }

        // Select another point randomly.
        std::uniform_real_distribution<>
            distribution;  // distribution of [0, 1)
        if constexpr (base::concepts::real_scalar_dense_vector<variable_type>) {
            using scalar_type = typename variable_type::Scalar;
            variable_type random_variable(dim_);
            for (index_type i = 0; i < dim_; ++i) {
                random_variable(i) = lower_(i) +
                    (upper_(i) - lower_(i)) *
                        static_cast<scalar_type>(
                            distribution(random_number_generator_));
            }
            evaluate_on(random_variable);
        } else {
            // Ordinary scalar case.
            const variable_type random_variable = lower_ +
                (upper_ - lower_) *
                    static_cast<value_type>(
                        distribution(random_number_generator_));
            evaluate_on(random_variable);
        }
    }

    //! Optimizer of lower bounds.
    dividing_rectangles<any_objective_function<value_type(variable_type)>>
        lower_bound_optimizer_{};

    //! Objective function.
    objective_function_type obj_fun_;

    //! Interpolator.
    interpolator_type interpolator_{};

    //! Variables of sample points.
    std::vector<variable_type> variables_{};

    //! Function values of sample points.
    Eigen::VectorX<value_type> values_{};

    //! Element-wise lower limit.
    variable_type lower_{};

    //! Element-wise upper limit.
    variable_type upper_{};

    //! Number of dimension.
    index_type dim_{0};

    //! Current optimal variable.
    variable_type opt_variable_{};

    //! Current optimal value.
    value_type opt_value_{};

    //! Number of iterations.
    index_type iterations_{0};

    //! Number of function evaluations.
    index_type evaluations_{0};

    //! Default value of the maximum number of function evaluations.
    static constexpr index_type default_max_evaluations = 20;

    //! Maximum number of function evaluations.
    index_type max_evaluations_{default_max_evaluations};

    //! Default value of the maximum number of evaluations of lower bounds.
    static constexpr index_type default_max_lower_bound_evaluations = 100;

    //! Default coefficient of the threshold of distances between sample points.
    static constexpr double default_distance_threshold_coeff = 0.01;

    //! Coefficient of the threshold of distances between sample points.
    double distance_threshold_coeff_{default_distance_threshold_coeff};

    //! Random number generator.
    std::mt19937 random_number_generator_{};
};

}  // namespace num_collect::opt
