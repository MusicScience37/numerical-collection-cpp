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

#include <algorithm>

#include "num_collect/base/norm.h"
#include "num_collect/constants/zero.h"               // IWYU pragma: keep
#include "num_collect/ode/concepts/formula_solver.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/problem.h"         // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of formulas in Runge-Kutta method.
 *
 * \tparam Derived Type of derived class.
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <typename Derived, concepts::problem Problem,
    concepts::formula_solver FormulaSolver>
class implicit_formula_base {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of solver of formula.
    using formula_solver_type = FormulaSolver;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Construct.
     *
     * \param[in] problem Problem.
     */
    explicit implicit_formula_base(const problem_type& problem = problem_type())
        : formula_solver_(problem) {}

    /*!
     * \brief Compute the next variable.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     */
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        derived().step(time, step_size, current, estimate);
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() -> problem_type& {
        return formula_solver_.problem();
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() const -> const problem_type& {
        return formula_solver_.problem();
    }

    /*!
     * \brief Set relative tolerance of residual norm.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_rel_residual_norm(scalar_type val) -> Derived& {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        tol_rel_residual_norm_ = val;
        return derived();
    }

    /*!
     * \brief Set absolute tolerance of residual norm.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_abs_residual_norm(scalar_type val) -> Derived& {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        tol_abs_residual_norm_ = val;
        return derived();
    }

protected:
    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }

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
     * \brief Convert coefficients.
     *
     * \tparam T Input type.
     * \param[in] val Input value.
     * \return Converted value.
     */
    template <typename T>
    static constexpr auto coeff(T val) -> scalar_type {
        return static_cast<scalar_type>(val);
    }

    /*!
     * \brief Create coefficients.
     *
     * \tparam T1 Input type.
     * \tparam T2 Input type.
     * \param[in] num Numerator.
     * \param[in] den Denominator.
     * \return Coefficient.
     */
    template <typename T1, typename T2>
    static constexpr auto coeff(T1 num, T2 den) -> scalar_type {
        return static_cast<scalar_type>(num) / static_cast<scalar_type>(den);
    }

    /*!
     * \brief Calculate tolerance of residual norm.
     *
     * \param[in] variable Variable.
     * \param[in] step_size Step size.
     * \return Tolerance of residual norm.
     */
    [[nodiscard]] auto tol_residual_norm(
        const variable_type& variable, scalar_type step_size) {
        return std::max(tol_abs_residual_norm_,
                   tol_rel_residual_norm_ * norm(variable)) /
            step_size;
    }

private:
    //! Solver of formula.
    formula_solver_type formula_solver_;

    //! Default relative tolerance of residual norm.
    static constexpr auto default_tol_rel_residual_norm =
        static_cast<scalar_type>(1e-8);

    //! Relative tolerance of residual norm.
    scalar_type tol_rel_residual_norm_{default_tol_rel_residual_norm};

    //! Default absolute tolerance of residual norm.
    static constexpr auto default_tol_abs_residual_norm =
        static_cast<scalar_type>(1e-8);

    //! Absolute tolerance of residual norm.
    scalar_type tol_abs_residual_norm_{default_tol_abs_residual_norm};
};

}  // namespace num_collect::ode::runge_kutta
