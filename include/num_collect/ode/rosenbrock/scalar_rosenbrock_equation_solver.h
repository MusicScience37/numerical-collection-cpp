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
 * \brief Definition of scalar_rosenbrock_equation_solver class.
 */
#pragma once

#include <cmath>
#include <limits>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode::rosenbrock {

/*!
 * \brief Class to solve equations in Rosenbrock methods for single-variate
 * case.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::single_variate_differentiable_problem Problem>
class scalar_rosenbrock_equation_solver {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobians.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");
    // TODO: Support is actually easy for this formula, but I want to test it
    // after implementation, so postpone the implementation.

    /*!
     * \brief Constructor.
     *
     * \param[in] inverted_jacobian_coeff Coefficient multiplied to Jacobian
     * matrices in inverted matrices.
     */
    explicit scalar_rosenbrock_equation_solver(
        const scalar_type& inverted_jacobian_coeff)
        : inverted_jacobian_coeff_(inverted_jacobian_coeff) {}

    /*!
     * \brief Update Jacobian and internal parameters.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     */
    void evaluate_and_update_jacobian(problem_type& problem,
        const scalar_type& time, const scalar_type& step_size,
        const variable_type& variable) {
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});
        jacobian_ = problem.jacobian();

        const auto inverted_value = static_cast<scalar_type>(1) -
            step_size * inverted_jacobian_coeff_ * jacobian_;
        using std::abs;
        if (abs(inverted_value) < std::numeric_limits<scalar_type>::epsilon()) {
            throw algorithm_failure(fmt::format(
                "Value to invert is too small: {}.", inverted_value));
        }
        inverted_coeff_ = static_cast<scalar_type>(1) / inverted_value;
    }

    /*!
     * \brief Multiply Jacobian to a value.
     *
     * \param[in] target Target.
     * \param[out] result Result.
     */
    void apply_jacobian(const variable_type& target, variable_type& result) {
        result = jacobian_ * target;
    }

    /*!
     * \brief Solve a linear equation.
     *
     * \param[in] rhs Right-hand-side value.
     * \param[out] result Result.
     */
    void solve(const variable_type& rhs, variable_type& result) {
        result = inverted_coeff_ * rhs;
    }

private:
    //! Jacobian.
    jacobian_type jacobian_{};

    //! Inverted coefficient.
    jacobian_type inverted_coeff_{};

    //! Coefficient multiplied to Jacobian in inverted values.
    scalar_type inverted_jacobian_coeff_{static_cast<scalar_type>(1)};
};

}  // namespace num_collect::ode::rosenbrock
