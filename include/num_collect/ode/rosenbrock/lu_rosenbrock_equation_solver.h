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
 * \brief Definition of lu_rosenbrock_equation_solver class.
 */
#pragma once

#include <optional>

#include <Eigen/Core>  // IWYU pragma: keep
#include <Eigen/LU>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"
#include "num_collect/ode/concepts/time_differentiable_problem.h"
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::rosenbrock {

/*!
 * \brief Class to solve equations in Rosenbrock methods using LU decomposition.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::multi_variate_differentiable_problem Problem>
class lu_rosenbrock_equation_solver {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobians.
    using jacobian_type = typename problem_type::jacobian_type;

    //! Type of the LU solver.
    using lu_solver_type = Eigen::PartialPivLU<jacobian_type>;

    //! Whether to use partial derivative with respect to time.
    static constexpr bool use_time_derivative =
        concepts::time_differentiable_problem<problem_type>;

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] inverted_jacobian_coeff Coefficient multiplied to Jacobian
     * matrices in inverted matrices.
     */
    explicit lu_rosenbrock_equation_solver(
        const scalar_type& inverted_jacobian_coeff)
        : inverted_jacobian_coeff_(inverted_jacobian_coeff) {}

    /*!
     * \brief Update Jacobian matrix and internal parameters.
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
            evaluation_type{.diff_coeff = true,
                .jacobian = true,
                .time_derivative = use_time_derivative,
                .mass = use_mass});
        jacobian_ = problem.jacobian();
        if constexpr (use_time_derivative) {
            time_derivative_ = problem.time_derivative();
        }

        const index_type variable_size = variable.size();
        if constexpr (use_mass) {
            lu_.compute(problem.mass() -
                step_size * inverted_jacobian_coeff_ * jacobian_);
        } else {
            lu_.compute(jacobian_type::Identity(variable_size, variable_size) -
                step_size * inverted_jacobian_coeff_ * jacobian_);
        }

        // TODO: Check that condition number is not so large.
    }

    /*!
     * \brief Multiply Jacobian matrix to a vector.
     *
     * \param[in] target Target.
     * \param[out] result Result.
     */
    void apply_jacobian(const variable_type& target, variable_type& result) {
        result = jacobian_ * target;
    }

    /*!
     * \brief Add a term of partial derivative with respect to time.
     *
     * \param[in] step_size Step size.
     * \param[in] coeff Coefficient in formula.
     * \param[in,out] target Target variable.
     */
    void add_time_derivative_term(const scalar_type& step_size,
        const scalar_type& coeff, variable_type& target) {
        if constexpr (use_time_derivative) {
            if (time_derivative_) {
                target += step_size * coeff * (*time_derivative_);
            }
        }
    }

    /*!
     * \brief Solve a linear equation.
     *
     * \param[in] rhs Right-hand-side value.
     * \param[out] result Result.
     */
    void solve(const variable_type& rhs, variable_type& result) {
        result = lu_.solve(rhs);
    }

private:
    //! Jacobian matrix.
    jacobian_type jacobian_{};

    //! Partial derivative with respect to time.
    std::optional<variable_type> time_derivative_{};

    //! LU solver.
    lu_solver_type lu_{};

    //! Coefficient multiplied to Jacobian matrices in inverted matrices.
    scalar_type inverted_jacobian_coeff_{static_cast<scalar_type>(1)};
};

}  // namespace num_collect::ode::rosenbrock
