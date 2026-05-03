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
 * \brief Definition of esdirk32_formula class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/adaptive_step_solver.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/concepts/update_equation_solver.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"
#include "num_collect/ode/runge_kutta/inexact_newton_update_equation_solver.h"
#include "num_collect/ode/runge_kutta/slope_calculator.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of ESDIRK 3/2 a formula in \cite Kvaerno2004.
 *
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <concepts::problem Problem,
    concepts::update_equation_solver FormulaSolver =
        inexact_newton_update_equation_solver<Problem>>
class esdirk32_formula
    : public implicit_formula_base<esdirk32_formula<Problem, FormulaSolver>,
          Problem, FormulaSolver> {
public:
    //! Type of this class.
    using this_type = esdirk32_formula<Problem, FormulaSolver>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<esdirk32_formula<Problem, FormulaSolver>, Problem,
            FormulaSolver>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::formula_solver;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 4;

    //! Order of this formula.
    static constexpr index_type order = 3;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 2;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::esdirk32_formula");

    /*!
     * \name Coefficients in Butcher array.
     *
     * - `a` is coefficients of intermediate variables in calculation of
     *   intermediate variables.
     * - `b` is coefficients of time in calculation of intermediate variables.
     * - `c` is coefficients of intermediate variables in calculation of
     *   estimates of next variables.
     */
    ///@{
    //! Coefficient in Butcher array.
    static constexpr auto ad = coeff(0.4358665215);  // diagonal coefficient
    static constexpr scalar_type a21 = ad;
    static constexpr scalar_type a31 =
        (-coeff(4) * ad * ad + coeff(6) * ad - coeff(1)) / (coeff(4) * ad);
    static constexpr scalar_type a32 =
        (-coeff(2) * ad + coeff(1)) / (coeff(4) * ad);
    static constexpr scalar_type a41 =
        (coeff(6) * ad - coeff(1)) / (coeff(12) * ad);
    static constexpr scalar_type a42 =
        (-coeff(1)) / ((coeff(24) * ad - coeff(12)) * ad);
    static constexpr scalar_type a43 =
        (-coeff(6) * ad * ad + coeff(6) * ad - coeff(1)) /
        (coeff(6) * ad - coeff(3));
    static constexpr scalar_type b2 = coeff(2) * ad;
    static constexpr scalar_type b3 = coeff(1);
    static constexpr scalar_type b4 = coeff(1);
    ///@}

    //! \copydoc runge_kutta::implicit_formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        formula_solver().update_jacobian(
            problem(), time, step_size, current, ad);

        slope_calculator_(problem(), time, current, k1_);

        z2_ = step_size * ad * k1_;
        formula_solver().init(
            time + b2 * step_size, step_size * a21 * k1_, z2_);
        formula_solver().solve();
        k2_ = (z2_ - formula_solver().solution_offset()) / (step_size * ad);

        z3_ = z2_;
        formula_solver().init(
            time + b3 * step_size, step_size * (a31 * k1_ + a32 * k2_), z3_);
        formula_solver().solve();
        k3_ = (z3_ - formula_solver().solution_offset()) / (step_size * ad);

        z4_ = z3_;
        formula_solver().init(time + b4 * step_size,
            step_size * (a41 * k1_ + a42 * k2_ + a43 * k3_), z4_);
        formula_solver().solve();

        estimate = current + z4_;
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
        step(time, step_size, current, estimate);
        error = z4_ - z3_;
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
    variable_type z2_{};
    variable_type z3_{};
    variable_type z4_{};
    ///@}

    //! Calculator of slopes.
    slope_calculator<problem_type> slope_calculator_;
};

/*!
 * \brief Class of solver using ESDIRK 3/2 a formula in \cite Kvaerno2004 with
 * adaptive step sizes.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using esdirk32_adaptive_step_solver =
    adaptive_step_solver<esdirk32_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
