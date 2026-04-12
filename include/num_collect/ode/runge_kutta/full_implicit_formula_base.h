/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of full_implicit_formula_base class.
 */
#pragma once

#include "num_collect/logging/logger.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Base class of formulas in Runge-Kutta method which are fully implicit.
 *
 * \tparam Derived Type of derived class.
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <typename Derived, concepts::problem Problem,
    // TODO FormulaSolver for this case does not have a concept for now.
    typename FormulaSolver>
class full_implicit_formula_base : public formula_base<Derived, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<Derived, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    //! Type of solver of formula.
    using formula_solver_type = FormulaSolver;

protected:
    using base_type::derived;

    /*!
     * \brief Constructor.
     *
     * \tparam FormulaSolverArgs Types of arguments of the constructor of the
     * solver of the formula.
     * \param[in] problem Problem.
     * \param[in] formula_solver_args Arguments of the constructor of the solver
     * of the formula.
     */
    template <typename... FormulaSolverArgs>
    explicit full_implicit_formula_base(
        const problem_type& problem = problem_type(),
        FormulaSolverArgs&&... formula_solver_args)
        : base_type(problem),
          formula_solver_(
              std::forward<FormulaSolverArgs>(formula_solver_args)...) {}

public:
    /*!
     * \brief Get solver of formula.
     *
     * \return Solver of formula.
     */
    [[nodiscard]] auto formula_solver() -> formula_solver_type& {
        return formula_solver_;
    }

    /*!
     * \brief Get solver of formula.
     *
     * \return Solver of formula.
     */
    [[nodiscard]] auto formula_solver() const -> const formula_solver_type& {
        return formula_solver_;
    }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val) -> Derived& {
        formula_solver_.tolerances(val);
        return derived();
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() const noexcept
        -> const num_collect::logging::logger& {
        return formula_solver_.logger();
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() noexcept -> num_collect::logging::logger& {
        return formula_solver_.logger();
    }

private:
    //! Solver of formula.
    formula_solver_type formula_solver_;
};

}  // namespace num_collect::ode::runge_kutta
