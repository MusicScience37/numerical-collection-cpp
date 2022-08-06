/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of initial_step_size_calculator class.
 */
#pragma once

#include <algorithm>
#include <cmath>  // IWYU pragma: keep
#include <string_view>
#include <tuple>

#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/concepts/formula.h"  // IWYU pragma: keep
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/impl/get_least_known_order.h"
#include "num_collect/ode/step_size_limits.h"

namespace num_collect::ode {

//! Log tag.
constexpr auto initial_step_size_calculator_log_tag =
    logging::log_tag_view("num_collect::ode::initial_step_size_calculator");

/*!
 * \brief Class to calculate initial step sizes \cite Hairer1993.
 *
 * \tparam Formula Type of the formula.
 */
template <concepts::formula Formula>
class initial_step_size_calculator : public logging::logging_mixin {
public:
    //! Type of formula.
    using formula_type = Formula;

    //! Type of problem.
    using problem_type = typename formula_type::problem_type;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Constructor.
     */
    initial_step_size_calculator()
        : logging::logging_mixin(initial_step_size_calculator_log_tag) {}

    /*!
     * \brief Calculate the initial step size.
     *
     * \param[in] problem Problem.
     * \param[in] initial_time Initial time.
     * \param[in] initial_variable Initial variable.
     * \param[in] limits Limits of the step size.
     * \param[in] tolerances Tolerances of errors.
     * \return Selected initial step size.
     */
    auto calculate(problem_type& problem, const scalar_type& initial_time,
        const variable_type& initial_variable,
        const step_size_limits<scalar_type>& limits,
        const error_tolerances<variable_type>& tolerances) const
        -> scalar_type {
        problem.evaluate_on(initial_time, initial_variable,
            evaluation_type{.diff_coeff = true});
        const variable_type initial_diff = problem.diff_coeff();

        const auto [step_size_from_diff, initial_diff_norm] =
            calculate_step_size_from_diff(
                initial_variable, initial_diff, tolerances);

        const scalar_type step_size_from_second_diff =
            calculate_step_size_from_second_diff(problem, initial_time,
                initial_variable, initial_diff, step_size_from_diff,
                initial_diff_norm, tolerances);

        const scalar_type step_size_without_limit =
            std::min(1e+2 * step_size_from_diff, step_size_from_second_diff);
        this->logger().trace()("Selection of step size without limits: {}",
            step_size_without_limit);

        const scalar_type final_step_size =
            limits.apply(step_size_without_limit);
        this->logger().trace()(
            "Final selection of step size: {}", step_size_without_limit);

        return final_step_size;
    }

private:
    /*!
     * \brief Calculate a step size from differential coefficients at the
     * initial time.
     *
     * \param[in] initial_variable Initial variable.
     * \param[in] initial_diff Initial differential coefficients.
     * \param[in] tolerances Tolerances of errors.
     * \return Step size and norm of initial differential coefficients.
     */
    auto calculate_step_size_from_diff(const variable_type& initial_variable,
        const variable_type& initial_diff,
        const error_tolerances<variable_type>& tolerances) const
        -> std::tuple<scalar_type, scalar_type> {
        // d0 in Hairer1993
        const scalar_type initial_variable_norm =
            tolerances.calc_norm(initial_variable, initial_variable);
        this->logger().trace()("Norm of variable: {}", initial_variable_norm);

        // d1 in Hairer1993
        const scalar_type initial_diff_norm =
            tolerances.calc_norm(initial_variable, initial_diff);
        this->logger().trace()(
            "Norm of first derivative: {}", initial_diff_norm);

        // h0 in Hairer1993
        const scalar_type step_size_from_diff =
            (initial_variable_norm >= static_cast<scalar_type>(1e-5) &&
                initial_diff_norm >= static_cast<scalar_type>(1e-5))
            ? static_cast<scalar_type>(1e-2) * initial_variable_norm /
                initial_diff_norm
            : static_cast<scalar_type>(1e-6);
        this->logger().trace()(
            "First estimate of step size using differential coefficient: {}",
            step_size_from_diff);

        return {step_size_from_diff, initial_diff_norm};
    }

    /*!
     * \brief Calculate a step size using a estimate of the second derivative.
     *
     * \param[in] problem Problem.
     * \param[in] initial_time Initial time.
     * \param[in] initial_variable Initial variable.
     * \param[in] initial_diff Initial differential coefficients.
     * \param[in] step_size_from_diff Step size calculated from differential
     * coefficients at the initial time.
     * \param[in] initial_diff_norm Norm of the initial differential
     * coefficients.
     * \param[in] tolerances Tolerances of errors.
     * \return Step size.
     */
    auto calculate_step_size_from_second_diff(problem_type& problem,
        const scalar_type& initial_time, const variable_type& initial_variable,
        const variable_type& initial_diff,
        const scalar_type& step_size_from_diff,
        const scalar_type& initial_diff_norm,
        const error_tolerances<variable_type>& tolerances) const {
        // y1 in Hairer1993 (Explicit Euler method)
        const variable_type euler_updated_variable =
            initial_variable + step_size_from_diff * initial_diff;

        problem.evaluate_on(initial_time + step_size_from_diff,
            euler_updated_variable, evaluation_type{.diff_coeff = true});
        const variable_type& euler_updated_diff = problem.diff_coeff();

        // d2 in Hairer1993 (Evaluation of second derivative)
        const scalar_type second_diff_norm =
            tolerances.calc_norm(
                initial_variable, euler_updated_diff - initial_diff) /
            step_size_from_diff;
        this->logger().trace()(
            "Norm of second derivative: {}", second_diff_norm);

        const scalar_type larger_norm =
            std::max(initial_diff_norm, second_diff_norm);
        constexpr scalar_type exponent_of_order = static_cast<scalar_type>(1) /
            static_cast<scalar_type>(
                impl::get_least_known_order<Formula>() + 1);
        // h1 in Hairer1993
        const scalar_type step_size_from_second_diff =
            (larger_norm > static_cast<scalar_type>(1e-15))
            ? std::pow(static_cast<scalar_type>(1e-2) / larger_norm,
                  exponent_of_order)
            : std::max(static_cast<scalar_type>(1e-6),
                  static_cast<scalar_type>(1e-3) * step_size_from_diff);
        this->logger().trace()(
            "Second estimate of step size using second derivative: {}",
            step_size_from_second_diff);

        return step_size_from_second_diff;
    }
};

}  // namespace num_collect::ode
