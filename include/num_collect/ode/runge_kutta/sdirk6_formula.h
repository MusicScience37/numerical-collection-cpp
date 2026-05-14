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
 * \brief Definition of sdirk6_formula class.
 */
#pragma once

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/adaptive_step_solver.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/concepts/update_equation_solver.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"
#include "num_collect/ode/runge_kutta/inexact_newton_update_equation_solver.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of 6th order SDIRK (singly diagonally implicit Runge-Kutta)
 * formula SDIRK(9,6)[1]SAL-[(9,5)A] in \cite Alamri2023.
 *
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <concepts::problem Problem,
    concepts::update_equation_solver FormulaSolver =
        inexact_newton_update_equation_solver<Problem>>
class sdirk6_formula
    : public implicit_formula_base<sdirk6_formula<Problem, FormulaSolver>,
          Problem, FormulaSolver> {
public:
    //! Type of this class.
    using this_type = sdirk6_formula<Problem, FormulaSolver>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<sdirk6_formula<Problem, FormulaSolver>, Problem,
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
    static constexpr index_type stages = 9;

    //! Order of this formula.
    static constexpr index_type order = 6;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 5;

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::runge_kutta::sdirk6_formula");

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

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
    static constexpr scalar_type ad =
        coeff(2.181277819449076e-01);  // diagonal coefficient
    static constexpr scalar_type a21 = coeff(-9.035148561194185e-02);
    static constexpr scalar_type a31 = coeff(1.729520391389366e-01);
    static constexpr scalar_type a32 = coeff(-3.536550103628203e-01);
    static constexpr scalar_type a41 = coeff(5.119998759191926e-01);
    static constexpr scalar_type a42 = coeff(2.896403322019248e-02);
    static constexpr scalar_type a43 = coeff(-1.440309456570937e-02);
    static constexpr scalar_type a51 = coeff(4.653034955067823e-03);
    static constexpr scalar_type a52 = coeff(-7.563581876659697e-02);
    static constexpr scalar_type a53 = coeff(2.172730307867122e-01);
    static constexpr scalar_type a54 = coeff(-2.065194287254723e-02);
    static constexpr scalar_type a61 = coeff(8.961455017624717e-01);
    static constexpr scalar_type a62 = coeff(1.392673277004985e-01);
    static constexpr scalar_type a63 = coeff(-1.869209797528052e-01);
    static constexpr scalar_type a64 = coeff(6.729710123717235e-02);
    static constexpr scalar_type a65 = coeff(-3.508919634421756e-01);
    static constexpr scalar_type a71 = coeff(5.529597018857514e-01);
    static constexpr scalar_type a72 = coeff(-4.393605797936621e-01);
    static constexpr scalar_type a73 = coeff(3.337040023250907e-01);
    static constexpr scalar_type a74 = coeff(-3.394265207784165e-02);
    static constexpr scalar_type a75 = coeff(-1.519474459125954e-01);
    static constexpr scalar_type a76 = coeff(2.138256610269428e-02);
    static constexpr scalar_type a81 = coeff(6.313603740364756e-01);
    static constexpr scalar_type a82 = coeff(7.247336196414658e-01);
    static constexpr scalar_type a83 = coeff(-4.321706254252584e-01);
    static constexpr scalar_type a84 = coeff(5.986113821824766e-01);
    static constexpr scalar_type a85 = coeff(-7.090871970343450e-01);
    static constexpr scalar_type a86 = coeff(-4.839866856969341e-01);
    static constexpr scalar_type a87 = coeff(3.783915629051305e-01);
    // a91 = 0
    static constexpr scalar_type a92 = coeff(-1.550445253086903e-01);
    static constexpr scalar_type a93 = coeff(1.945184786607890e-01);
    static constexpr scalar_type a94 = coeff(6.351564027920301e-01);
    static constexpr scalar_type a95 = coeff(8.117227866417299e-01);
    static constexpr scalar_type a96 = coeff(1.107361086915851e-01);
    static constexpr scalar_type a97 = coeff(-4.953046924144789e-01);
    static constexpr scalar_type a98 = coeff(-3.199123410078724e-01);

    static constexpr scalar_type b1 = ad;
    static constexpr scalar_type b2 = a21 + ad;
    static constexpr scalar_type b3 = a31 + a32 + ad;
    static constexpr scalar_type b4 = a41 + a42 + a43 + ad;
    static constexpr scalar_type b5 = a51 + a52 + a53 + a54 + ad;
    static constexpr scalar_type b6 = a61 + a62 + a63 + a64 + a65 + ad;
    static constexpr scalar_type b7 = a71 + a72 + a73 + a74 + a75 + a76 + ad;
    static constexpr scalar_type b8 =
        a81 + a82 + a83 + a84 + a85 + a86 + a87 + ad;
    static constexpr scalar_type b9 =
        a92 + a93 + a94 + a95 + a96 + a97 + a98 + ad;

    // c1 = a91 = 0
    static constexpr scalar_type c2 = a92;
    static constexpr scalar_type c3 = a93;
    static constexpr scalar_type c4 = a94;
    static constexpr scalar_type c5 = a95;
    static constexpr scalar_type c6 = a96;
    static constexpr scalar_type c7 = a97;
    static constexpr scalar_type c8 = a98;
    static constexpr scalar_type c9 = ad;

    // cw1 = a91 = 0
    static constexpr scalar_type cw2 = coeff(7.366155582789420e-02);
    static constexpr scalar_type cw3 = coeff(1.035273972622287e-01);
    static constexpr scalar_type cw4 = coeff(1.002474819354989e+00);
    static constexpr scalar_type cw5 = coeff(3.613772892500572e-01);
    static constexpr scalar_type cw6 = coeff(-7.854259299613646e-01);
    static constexpr scalar_type cw7 = coeff(-1.704990479607844e-02);
    static constexpr scalar_type cw8 = coeff(2.963212522147690e-01);
    static constexpr scalar_type cw9 = coeff(-3.488647915249531e-02);

    // ce1 = c1 - cw1 = 0
    static constexpr scalar_type ce2 = c2 - cw2;
    static constexpr scalar_type ce3 = c3 - cw3;
    static constexpr scalar_type ce4 = c4 - cw4;
    static constexpr scalar_type ce5 = c5 - cw5;
    static constexpr scalar_type ce6 = c6 - cw6;
    static constexpr scalar_type ce7 = c7 - cw7;
    static constexpr scalar_type ce8 = c8 - cw8;
    static constexpr scalar_type ce9 = c9 - cw9;
    ///@}

    //! \copydoc runge_kutta::implicit_formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        formula_solver().update_jacobian(
            problem(), time, step_size, current, ad);

        if constexpr (use_mass) {
            if constexpr (base::concepts::real_scalar_dense_vector<
                              variable_type>) {
                z1_ = variable_type::Zero(current.size());
            } else {
                z1_ = static_cast<variable_type>(0);
            }
        } else {
            z1_ = step_size * ad * problem().diff_coeff();
        }
        if constexpr (base::concepts::real_scalar_dense_vector<variable_type>) {
            formula_solver().init(time + b1 * step_size,
                variable_type::Zero(current.size()), z1_);
        } else {
            formula_solver().init(
                time + b1 * step_size, static_cast<variable_type>(0), z1_);
        }
        formula_solver().solve();
        k1_ = z1_ / (step_size * ad);

        z2_ = z1_;
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
            step_size *
                (a61 * k1_ + a62 * k2_ + a63 * k3_ + a64 * k4_ + a65 * k5_),
            z6_);
        formula_solver().solve();
        k6_ = (z6_ - formula_solver().solution_offset()) / (step_size * ad);

        z7_ = z6_;
        formula_solver().init(time + b7 * step_size,
            step_size *
                (a71 * k1_ + a72 * k2_ + a73 * k3_ + a74 * k4_ + a75 * k5_ +
                    a76 * k6_),
            z7_);
        formula_solver().solve();
        k7_ = (z7_ - formula_solver().solution_offset()) / (step_size * ad);

        z8_ = z7_;
        formula_solver().init(time + b8 * step_size,
            step_size *
                (a81 * k1_ + a82 * k2_ + a83 * k3_ + a84 * k4_ + a85 * k5_ +
                    a86 * k6_ + a87 * k7_),
            z8_);
        formula_solver().solve();
        k8_ = (z8_ - formula_solver().solution_offset()) / (step_size * ad);

        z9_ = z8_;
        formula_solver().init(time + b9 * step_size,
            step_size *
                (a92 * k2_ + a93 * k3_ + a94 * k4_ + a95 * k5_ + a96 * k6_ +
                    a97 * k7_ + a98 * k8_),
            z9_);
        formula_solver().solve();

        estimate = current + z9_;
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
        k9_ = (z9_ - formula_solver().solution_offset()) / (step_size * ad);

        error = step_size *
            (ce2 * k2_ + ce3 * k3_ + ce4 * k4_ + ce5 * k5_ + ce6 * k6_ +
                ce7 * k7_ + ce8 * k8_ + ce9 * k9_);
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
    variable_type k9_{};
    variable_type z1_{};
    variable_type z2_{};
    variable_type z3_{};
    variable_type z4_{};
    variable_type z5_{};
    variable_type z6_{};
    variable_type z7_{};
    variable_type z8_{};
    variable_type z9_{};
    ///@}
};

/*!
 * \brief Class of solver using 6th order SDIRK (singly diagonally implicit
 * Runge-Kutta) formula SDIRK(9,6)[1]SAL-[(9,5)A] in \cite Alamri2023.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using sdirk6_adaptive_step_solver =
    adaptive_step_solver<sdirk6_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
