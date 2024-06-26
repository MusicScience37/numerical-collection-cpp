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
 * \brief Definition of ark43_esdirk_formula class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/concepts/update_equation_solver.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/inexact_newton_update_equation_solver.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of ARK4(3)6L[2]SA-ESDIRK formula in \cite Kennedy2003 .
 *
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <concepts::problem Problem,
    concepts::update_equation_solver FormulaSolver =
        inexact_newton_update_equation_solver<Problem>>
class ark43_esdirk_formula
    : public implicit_formula_base<ark43_esdirk_formula<Problem, FormulaSolver>,
          Problem, FormulaSolver> {
public:
    //! Type of this class.
    using this_type = ark43_esdirk_formula<Problem, FormulaSolver>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<ark43_esdirk_formula<Problem, FormulaSolver>,
            Problem, FormulaSolver>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;
    using base_type::formula_solver;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 6;

    //! Order of this formula.
    static constexpr index_type order = 4;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 3;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::ark43_esdirk_formula");

    /*!
     * \name Coefficients in Butcher array.
     *
     * - `a` is coefficients of intermidiate variables in calculation of
     *   intermidiate variables.
     * - `b` is coefficients of time in calculation of intermidiate variables.
     * - `c` is coefficients of intermidiate variables in calculation of
     *   estimates of next variables.
     */
    ///@{
    //! Coefficient in Butcher array.
    static constexpr scalar_type ad = coeff(1, 4);  // diagonal coefficient
    static constexpr scalar_type a21 = coeff(1, 4);
    static constexpr scalar_type a31 = coeff(8611, 62500);
    static constexpr scalar_type a32 = coeff(-1743, 31250);
    static constexpr scalar_type a41 = coeff(5012029, 34652500);
    static constexpr scalar_type a42 = coeff(-654441, 2922500);
    static constexpr scalar_type a43 = coeff(174375, 388108);
    static constexpr scalar_type a51 = coeff(15267082809, 155376265600);
    static constexpr scalar_type a52 = coeff(-71443401, 120774400);
    static constexpr scalar_type a53 = coeff(730878875, 902184768);
    static constexpr scalar_type a54 = coeff(2285395, 8070912);
    static constexpr scalar_type a61 = coeff(82889, 524892);
    // a62 = 0
    static constexpr scalar_type a63 = coeff(15625, 83664);
    static constexpr scalar_type a64 = coeff(69875, 102672);
    static constexpr scalar_type a65 = coeff(-2260, 8211);

    // b1 = 0
    static constexpr scalar_type b2 = coeff(1, 2);
    static constexpr scalar_type b3 = coeff(83, 250);
    static constexpr scalar_type b4 = coeff(31, 50);
    static constexpr scalar_type b5 = coeff(17, 20);
    static constexpr scalar_type b6 = coeff(1);

    static constexpr scalar_type c1 = a61;
    // c2 = 0
    static constexpr scalar_type c3 = a63;
    static constexpr scalar_type c4 = a64;
    static constexpr scalar_type c5 = a65;
    static constexpr scalar_type c6 = ad;

    static constexpr scalar_type cw1 = coeff(4586570599, 29645900160);
    // cw2 = 0
    static constexpr scalar_type cw3 = coeff(178811875, 945068544);
    static constexpr scalar_type cw4 = coeff(814220225, 1159782912);
    static constexpr scalar_type cw5 = coeff(-3700637, 11593932);
    static constexpr scalar_type cw6 = coeff(61727, 225920);

    static constexpr scalar_type ce1 = c1 - cw1;
    // ce2 = 0
    static constexpr scalar_type ce3 = c3 - cw3;
    static constexpr scalar_type ce4 = c4 - cw4;
    static constexpr scalar_type ce5 = c5 - cw5;
    static constexpr scalar_type ce6 = c6 - cw6;
    ///@}

    //! \copydoc runge_kutta::implicit_formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        formula_solver().update_jacobian(
            problem(), time, step_size, current, ad);

        problem().evaluate_on(
            time, current, evaluation_type{.diff_coeff = true});
        k1_ = problem().diff_coeff();

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
        k4_ = (z4_ - formula_solver().solution_offset()) / (step_size * ad);

        z5_ = z4_;
        formula_solver().init(time + b5 * step_size,
            step_size * (a51 * k1_ + a52 * k2_ + a53 * k3_ + a54 * k4_), z5_);
        formula_solver().solve();
        k5_ = (z5_ - formula_solver().solution_offset()) / (step_size * ad);

        z6_ = z5_;
        formula_solver().init(time + b6 * step_size,
            step_size * (a61 * k1_ + a63 * k3_ + a64 * k4_ + a65 * k5_), z6_);
        formula_solver().solve();

        estimate = current + z6_;
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
        k6_ = (z6_ - formula_solver().solution_offset()) / (step_size * ad);

        error = step_size *
            (ce1 * k1_ + ce3 * k3_ + ce4 * k4_ + ce5 * k5_ + ce6 * k6_);
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
    variable_type k4_{};
    variable_type k5_{};
    variable_type k6_{};
    variable_type z2_{};
    variable_type z3_{};
    variable_type z4_{};
    variable_type z5_{};
    variable_type z6_{};
    ///@}
};

/*!
 * \brief Class of solver using ARK4(3)6L[2]SA-ESDIRK formula in
 * \cite Kennedy2003 .
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using ark43_esdirk_solver = embedded_solver<ark43_esdirk_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
