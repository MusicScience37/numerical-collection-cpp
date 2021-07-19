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
 * \brief Definition of tanaka2_formula class.
 */
#pragma once

#include "num_collect/ode/runge_kutta/embedded_solver.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"
#include "num_collect/ode/runge_kutta/semi_implicit_formula_solver.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of Tanaka Formula 2.
 *
 * \tparam Problem Type of problem.
 */
template <typename Problem, typename StrategyTag>
class tanaka2_formula
    : public implicit_formula_base<tanaka2_formula<Problem, StrategyTag>,
          Problem, semi_implicit_formula_solver<Problem, StrategyTag>> {
public:
    //! Type of this class.
    using this_type = tanaka2_formula<Problem, StrategyTag>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<tanaka2_formula<Problem, StrategyTag>, Problem,
            semi_implicit_formula_solver<Problem, StrategyTag>>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;
    using base_type::formula_solver;
    using base_type::tol_residual_norm;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 3;

    //! Order of this formula.
    static constexpr index_type order = 4;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 2;

    /*!
     * \name Coefficients in Butcher array.
     */
    ///@{
    //! Coefficnet in Butcher array.
    static constexpr scalar_type a11 = coeff(133, 100);
    static constexpr scalar_type a21 = coeff(-5400, 18167);
    static constexpr scalar_type a22 = coeff(28967, 36334);
    static constexpr scalar_type a31 = coeff(133, 50);
    static constexpr scalar_type a32 = coeff(-108, 25);
    static constexpr scalar_type a33 = coeff(133, 100);

    static constexpr scalar_type b1 = coeff(133, 100);
    static constexpr scalar_type b2 = coeff(1, 2);
    static constexpr scalar_type b3 = coeff(-33, 100);

    static constexpr scalar_type c1 = coeff(1250, 20667);
    static constexpr scalar_type c2 = coeff(18167, 20667);
    static constexpr scalar_type c3 = coeff(1250, 20667);

    static constexpr scalar_type cw2 = coeff(1);

    static constexpr scalar_type ce1 = c1;
    static constexpr scalar_type ce2 = c2 - cw2;
    static constexpr scalar_type ce3 = c3;
    ///@}

    //! \copydoc runge_kutta::implicit_formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        variable_type unused;
        step_embedded(time, step_size, current, estimate, unused);
    }

    /*!
     * \brief Compute the next variable and weak estimate of it with embedded
     * formula.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     * \param[out] error Estimate of error.
     */
    void step_embedded(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate,
        variable_type& error) {
        formula_solver().tol_residual_norm(
            tol_residual_norm(current, step_size));

        formula_solver().solve(time + b1 * step_size, step_size, current, a11);
        k1_ = formula_solver().k();

        formula_solver().solve(time + b2 * step_size, step_size,
            current + step_size * a21 * k1_, a22);
        k2_ = formula_solver().k();

        formula_solver().solve(time + b3 * step_size, step_size,
            current + step_size * (a31 * k1_ + a32 * k2_), a33);
        k3_ = formula_solver().k();

        estimate = current + step_size * (c1 * k1_ + c2 * k2_ + c3 * k3_);
        error = step_size * (ce1 * k1_ + ce2 * k2_ + ce3 * k3_);
    }

private:
    /*!
     * \name Intermediate variables.
     */
    ///@{
    //! Intermediate variable.
    variable_type k1_{};
    variable_type k2_{};
    variable_type k3_{};
    ///@}
};

/*!
 * \brief Class of solver using Tanaka Formula 2.
 *
 * \tparam Problem Type of problem.
 */
template <typename Problem>
using tanaka2_solver = embedded_solver<tanaka2_formula<Problem,
    implicit_formula_solver_strategies::modified_newton_raphson_tag>>;

}  // namespace num_collect::ode::runge_kutta
