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
 * \brief Definition of semi_implicit_formula_solver namespace.
 */
#pragma once

#include <Eigen/LU>
#include <limits>
#include <type_traits>

#include "num_collect/constants/zero.h"
#include "num_collect/ode/runge_kutta/implicit_formula_solver_strategies.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/index_type.h"
#include "num_collect/util/is_eigen_matrix.h"
#include "num_collect/util/is_eigen_vector.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of solvers of semi-implicit formulas.
 *
 * \tparam Problem Type of problem.
 * \tparam StrategyTag Type of tag of strategy (in
 * implicit_formula_solver_strategies namespace).
 */
template <typename Problem, typename StrategyTag, typename = void>
class semi_implicit_formula_solver;

/*!
 * \brief Class of solvers of semi-implicit formulas.
 *
 * \tparam Problem Type of problem.
 */
template <typename Problem>
class semi_implicit_formula_solver<Problem,
    implicit_formula_solver_strategies::modified_newton_raphson_tag,
    std::enable_if_t<is_eigen_vector_v<typename Problem::variable_type> &&
        is_eigen_matrix_v<typename Problem::jacobian_type>>> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    /*!
     * \brief Construct.
     *
     * \param[in] problem Problem.
     */
    explicit semi_implicit_formula_solver(const problem_type& problem)
        : problem_{problem} {}

    /*!
     * \brief Solve.
     *
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     * \param[in] k_coeff Coefficient for intermidiate variable.
     */
    void solve(scalar_type time, scalar_type step_size,
        const variable_type& variable, scalar_type k_coeff) {
        const index_type dim = variable.size();

        problem_.evaluate_on(time, variable, true);
        lu_.compute(jacobian_type::Identity(dim, dim) -
            step_size * k_coeff * problem_.jacobian());
        k_ = problem_.diff_coeff();

        constexpr index_type max_iterations = 1000;  // safe guard
        for (index_type i = 0; i <= max_iterations; ++i) {
            problem_.evaluate_on(
                time, variable + step_size * k_coeff * k_, false);
            residual_ = k_ - problem_.diff_coeff();
            residual_norm_ = residual_.norm();
            if (residual_norm_ < tol_residual_norm_) {
                break;
            }
            k_ -= lu_.solve(residual_);
        }
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() -> problem_type& { return problem_; }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() const -> const problem_type& {
        return problem_;
    }

    /*!
     * \brief Get the intermidiate variable.
     *
     * \return Intermidiate variable.
     */
    [[nodiscard]] auto k() const -> const variable_type& { return k_; }

    /*!
     * \brief Get residual norm.
     *
     * \return Residual norm.
     */
    [[nodiscard]] auto residual_norm() const -> scalar_type {
        return residual_norm_;
    }

    /*!
     * \brief Set tolerance of residual norm.
     *
     * \param[in] val Value.
     */
    void tol_residual_norm(scalar_type val) {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        tol_residual_norm_ = val;
    }

private:
    //! Problem
    problem_type problem_;

    //! Solver of LU decomposition.
    Eigen::PartialPivLU<jacobian_type> lu_{};

    //! Intermidiate variable.
    variable_type k_{};

    //! Residual vector.
    variable_type residual_{};

    //! Residual norm.
    scalar_type residual_norm_{std::numeric_limits<scalar_type>::infinity()};

    //! Default tolerance of residual norm.
    static constexpr auto default_tol_residual_norm =
        static_cast<scalar_type>(1e-8);

    //! Tolerance of residual norm.
    scalar_type tol_residual_norm_{default_tol_residual_norm};
};

}  // namespace num_collect::ode::runge_kutta
