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

#include <cmath>
#include <type_traits>
#include <vector>

#include <Eigen/Core>

#include "num_collect/base/get_size.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/function_object_wrapper.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/rbf/global_exact_rbf_interpolator.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/is_eigen_vector.h"

namespace num_collect::opt {

//! Tag of gaussian_process_optimizer.
inline constexpr auto gaussian_process_optimizer_tag =
    logging::log_tag_view("num_collect::opt::gaussian_process_optimizer");

/*!
 * \brief Class of Gaussian process optimization \cite Srinivas2010.
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
    }

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        if constexpr (is_eigen_vector_v<variable_type>) {
            NUM_COLLECT_ASSERT(get_size(lower) == get_size(upper));
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

        const auto lower_bound_function =
            [this](const variable_type& variable) -> value_type {
            using std::log;
            using std::sqrt;

            const auto [mean, variance] =
                interpolator_.evaluate_mean_and_variance_on(
                    variable, variables_);
            const auto variance_coeff = static_cast<value_type>(0.4) *
                log(static_cast<value_type>(evaluations_) *
                    static_cast<value_type>(evaluations_) *
                    constants::pi<value_type> * constants::pi<value_type> /
                    static_cast<value_type>(0.6));
            return mean - sqrt(variance_coeff * variance);
        };
        const auto lower_bound_objective_function =
            make_function_object_wrapper<value_type(variable_type)>(
                lower_bound_function);
        dividing_rectangles<
            std::decay_t<decltype(lower_bound_objective_function)>>
            lower_bound_optimizer{lower_bound_objective_function};

        this->configure_child_algorithm_logger_if_exists(lower_bound_optimizer);
        lower_bound_optimizer.max_evaluations(max_lower_bound_evaluations_);
        lower_bound_optimizer.init(lower_, upper_);
        lower_bound_optimizer.solve();

        evaluate_on(lower_bound_optimizer.opt_variable());

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
        NUM_COLLECT_ASSERT(value > 0);
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
        NUM_COLLECT_ASSERT(value > 0);
        max_lower_bound_evaluations_ = value;
        return *this;
    }

private:
    /*!
     * \brief Type of interpolator.
     */
    using interpolator_type =
        rbf::global_exact_rbf_interpolator<variable_type, value_type>;

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

    //! Maximum number of function evaluations of lower bounds.
    index_type max_lower_bound_evaluations_{
        default_max_lower_bound_evaluations};
};

}  // namespace num_collect::opt
