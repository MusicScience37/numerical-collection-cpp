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
 * \brief Definition of rosenbrock_formula_base class.
 */
#pragma once

#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"  // IWYU pragma: keep
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/formula_base.h"

namespace num_collect::ode::rosenbrock {

/*!
 * \brief Base class of formulas in Rosenbrock method.
 *
 * \tparam Derived Type of the derived class.
 * \tparam Problem Type of the problem.
 * \tparam EquationSolver Type of class to solve equations in Rosenbrock
 * methods.
 */
template <typename Derived, concepts::problem Problem,
    concepts::rosenbrock_equation_solver EquationSolver>
class rosenbrock_formula_base : public formula_base<Derived, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<Derived, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    //! Type of class to solve equations in Rosenbrock methods.
    using equation_solver_type = EquationSolver;

protected:
    using base_type::coeff;
    using base_type::derived;

public:
    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     * \param[in] inverted_jacobian_coeff Coefficient multiplied to Jacobian
     * matrices in inverted matrices.
     */
    rosenbrock_formula_base(
        const problem_type& problem, const scalar_type& inverted_jacobian_coeff)
        : base_type(problem), solver_(inverted_jacobian_coeff) {}

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val) -> Derived& {
        if constexpr (requires(equation_solver_type& solver,
                          const error_tolerances<variable_type>& val) {
                          solver.tolerances(val);
                      }) {
            solver_.tolerances(val);
        }
        return derived();
    }

    /*!
     * \brief Access the solver of equations in Rosenbrock methods.
     *
     * \return Solver.
     */
    [[nodiscard]] auto equation_solver() const -> const equation_solver_type& {
        return solver_;
    }

    /*!
     * \brief Access the solver of equations in Rosenbrock methods.
     *
     * \return Solver.
     */
    [[nodiscard]] auto equation_solver() -> equation_solver_type& {
        return solver_;
    }

private:
    //! Solver of equations in Rosenbrock methods.
    equation_solver_type solver_;
};

}  // namespace num_collect::ode::rosenbrock
