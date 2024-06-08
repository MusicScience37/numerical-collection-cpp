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
 * \brief Definition of implicit_formula_base class.
 */
#pragma once

#include "num_collect/logging/logger.h"
#include "num_collect/ode/concepts/ode_equation_solver.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of formulas in Runge-Kutta method.
 *
 * \tparam Derived Type of derived class.
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <typename Derived, concepts::problem Problem,
    concepts::ode_equation_solver FormulaSolver>
class implicit_formula_base : public formula_base<Derived, Problem> {
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

public:
    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     */
    explicit implicit_formula_base(const problem_type& problem = problem_type())
        : base_type(problem), formula_solver_() {}

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
