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
 * \brief Definition of ark54_esdirk_formula class.
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
 * \brief Class of ARK5(4)8L[2]SA-ESDIRK formula in \cite Kennedy2003 .
 *
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <concepts::problem Problem,
    concepts::update_equation_solver FormulaSolver =
        inexact_newton_update_equation_solver<Problem>>
class ark54_esdirk_formula
    : public implicit_formula_base<ark54_esdirk_formula<Problem, FormulaSolver>,
          Problem, FormulaSolver> {
public:
    //! Type of this class.
    using this_type = ark54_esdirk_formula<Problem, FormulaSolver>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<ark54_esdirk_formula<Problem, FormulaSolver>,
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
    static constexpr index_type stages = 8;

    //! Order of this formula.
    static constexpr index_type order = 5;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 4;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::ark54_esdirk_formula");

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
    static constexpr scalar_type ad = coeff(41, 200);  // diagonal coefficient
    static constexpr scalar_type a21 = ad;
    static constexpr scalar_type a31 = coeff(41, 400);
    static constexpr scalar_type a32 = coeff(-567603406766, 11931857230679);
    static constexpr scalar_type a41 = coeff(683785636431, 9252920307686);
    // a42 = 0
    static constexpr scalar_type a43 = coeff(-110385047103, 1367015193373);
    static constexpr scalar_type a51 = coeff(3016520224154, 10081342136671);
    // a52 = 0
    static constexpr scalar_type a53 = coeff(30586259806659, 12414158314087);
    static constexpr scalar_type a54 = coeff(-22760509404356, 11113319521817);
    static constexpr scalar_type a61 = coeff(218866479029, 1489978393911);
    // a62 = 0
    static constexpr scalar_type a63 = coeff(638256894668, 5436446318841);
    static constexpr scalar_type a64 = coeff(-1179710474555, 5321154724896);
    static constexpr scalar_type a65 = coeff(-60928119172, 8023461067671);
    static constexpr scalar_type a71 = coeff(1020004230633, 5715676835656);
    // a72 = 0
    static constexpr scalar_type a73 = coeff(25762820946817, 25263940353407);
    static constexpr scalar_type a74 = coeff(-2161375909145, 9755907335909);
    static constexpr scalar_type a75 = coeff(-211217309593, 5846859502534);
    static constexpr scalar_type a76 = coeff(-4269925059573, 7827059040749);
    static constexpr scalar_type a81 = coeff(-872700587467, 9133579230613);
    // a82 = 0
    // a83 = 0
    static constexpr scalar_type a84 = coeff(22348218063261, 9555858737531);
    static constexpr scalar_type a85 = coeff(-1143369518992, 8141816002931);
    static constexpr scalar_type a86 = coeff(-39379526789629, 19018526304540);
    static constexpr scalar_type a87 = coeff(32727382324388, 42900044865799);

    // b1 = 0
    static constexpr scalar_type b2 = coeff(41, 100);
    static constexpr scalar_type b3 = coeff(2935347310677, 11292855782101);
    static constexpr scalar_type b4 = coeff(1426016391358, 7196633302097);
    static constexpr scalar_type b5 = coeff(92, 100);
    static constexpr scalar_type b6 = coeff(24, 100);
    static constexpr scalar_type b7 = coeff(3, 5);
    static constexpr scalar_type b8 = coeff(1);

    static constexpr scalar_type c1 = a81;
    // c2 = 0
    // c3 = 0
    static constexpr scalar_type c4 = a84;
    static constexpr scalar_type c5 = a85;
    static constexpr scalar_type c6 = a86;
    static constexpr scalar_type c7 = a87;
    static constexpr scalar_type c8 = ad;

    static constexpr scalar_type cw1 = coeff(-975461918565, 9796059967033);
    // cw2 = 0
    // cw3 = 0
    static constexpr scalar_type cw4 = coeff(78070527104295, 32432590147079);
    static constexpr scalar_type cw5 = coeff(-548382580838, 3424219808633);
    static constexpr scalar_type cw6 = coeff(-33438840321285, 15594753105479);
    static constexpr scalar_type cw7 = coeff(3629800801594, 4656183773603);
    static constexpr scalar_type cw8 = coeff(4035322873751, 18575991585200);

    static constexpr scalar_type ce1 = c1 - cw1;
    // ce2 = 0
    // ce3 = 0
    static constexpr scalar_type ce4 = c4 - cw4;
    static constexpr scalar_type ce5 = c5 - cw5;
    static constexpr scalar_type ce6 = c6 - cw6;
    static constexpr scalar_type ce7 = c7 - cw7;
    static constexpr scalar_type ce8 = c8 - cw8;
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
        formula_solver().init(
            time + b4 * step_size, step_size * (a41 * k1_ + a43 * k3_), z4_);
        formula_solver().solve();
        k4_ = (z4_ - formula_solver().solution_offset()) / (step_size * ad);

        z5_ = z4_;
        formula_solver().init(time + b5 * step_size,
            step_size * (a51 * k1_ + a53 * k3_ + a54 * k4_), z5_);
        formula_solver().solve();
        k5_ = (z5_ - formula_solver().solution_offset()) / (step_size * ad);

        z6_ = z5_;
        formula_solver().init(time + b6 * step_size,
            step_size * (a61 * k1_ + a63 * k3_ + a64 * k4_ + a65 * k5_), z6_);
        formula_solver().solve();
        k6_ = (z6_ - formula_solver().solution_offset()) / (step_size * ad);

        z7_ = z6_;
        formula_solver().init(time + b7 * step_size,
            step_size *
                (a71 * k1_ + a73 * k3_ + a74 * k4_ + a75 * k5_ + a76 * k6_),
            z7_);
        formula_solver().solve();
        k7_ = (z7_ - formula_solver().solution_offset()) / (step_size * ad);

        z8_ = z7_;
        formula_solver().init(time + b8 * step_size,
            step_size *
                (a81 * k1_ + a84 * k4_ + a85 * k5_ + a86 * k6_ + a87 * k7_),
            z8_);
        formula_solver().solve();

        estimate = current + z8_;
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
        k8_ = (z8_ - formula_solver().solution_offset()) / (step_size * ad);

        error = step_size *
            (ce1 * k1_ + ce4 * k4_ + ce5 * k5_ + ce6 * k6_ + ce7 * k7_ +
                ce8 * k8_);
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
    variable_type k7_{};
    variable_type k8_{};
    variable_type z2_{};
    variable_type z3_{};
    variable_type z4_{};
    variable_type z5_{};
    variable_type z6_{};
    variable_type z7_{};
    variable_type z8_{};
    ///@}
};

/*!
 * \brief Class of solver using ARK5(4)6L[2]SA-ESDIRK formula in
 * \cite Kennedy2003 .
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using ark54_esdirk_solver = embedded_solver<ark54_esdirk_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
