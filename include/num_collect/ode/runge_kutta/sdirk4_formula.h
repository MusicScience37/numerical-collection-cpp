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
 * \brief Definition of sdirk4_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/update_equation_solver.h"  // IWYU pragma: keep
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/inexact_newton_update_equation_solver.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of 4th order SDIRK (singly diagonally implicit Runge-Kutta)
 * formula in \cite Hairer1991.
 *
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <concepts::problem Problem,
    concepts::update_equation_solver FormulaSolver =
        inexact_newton_update_equation_solver<Problem>>
class sdirk4_formula
    : public implicit_formula_base<sdirk4_formula<Problem, FormulaSolver>,
          Problem, FormulaSolver> {
public:
    //! Type of this class.
    using this_type = sdirk4_formula<Problem, FormulaSolver>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<sdirk4_formula<Problem, FormulaSolver>, Problem,
            FormulaSolver>;

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
    static constexpr index_type stages = 5;

    //! Order of this formula.
    static constexpr index_type order = 4;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 3;

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::runge_kutta::sdirk4_formula");

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
    static constexpr scalar_type a21 = coeff(1, 2);
    static constexpr scalar_type a31 = coeff(17, 50);
    static constexpr scalar_type a32 = coeff(-1, 25);
    static constexpr scalar_type a41 = coeff(371, 1360);
    static constexpr scalar_type a42 = coeff(-137, 2720);
    static constexpr scalar_type a43 = coeff(15, 544);
    static constexpr scalar_type a51 = coeff(25, 24);
    static constexpr scalar_type a52 = coeff(-49, 48);
    static constexpr scalar_type a53 = coeff(125, 16);
    static constexpr scalar_type a54 = coeff(-85, 12);

    static constexpr scalar_type b1 = coeff(1, 4);
    static constexpr scalar_type b2 = coeff(3, 4);
    static constexpr scalar_type b3 = coeff(11, 20);
    static constexpr scalar_type b4 = coeff(1, 2);
    static constexpr scalar_type b5 = coeff(1);

    static constexpr scalar_type c1 = a51;
    static constexpr scalar_type c2 = a52;
    static constexpr scalar_type c3 = a53;
    static constexpr scalar_type c4 = a54;
    static constexpr scalar_type c5 = ad;

    static constexpr scalar_type cw1 = coeff(59, 48);
    static constexpr scalar_type cw2 = coeff(-17, 96);
    static constexpr scalar_type cw3 = coeff(225, 32);
    static constexpr scalar_type cw4 = coeff(-85, 12);
    static constexpr scalar_type cw5 = coeff(0);

    static constexpr scalar_type ce1 = c1 - cw1;
    static constexpr scalar_type ce2 = c2 - cw2;
    static constexpr scalar_type ce3 = c3 - cw3;
    // ce4 = 0
    static constexpr scalar_type ce5 = c5 - cw5;
    ///@}

    //! \copydoc runge_kutta::implicit_formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        formula_solver().update_jacobian(
            problem(), time, step_size, current, ad);

        z1_ = step_size * ad * problem().diff_coeff();
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

        estimate = current + z5_;
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
        k5_ = (z5_ - formula_solver().solution_offset()) / (step_size * ad);

        error = step_size * (ce1 * k1_ + ce2 * k2_ + ce3 * k3_ + ce5 * k5_);
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
    variable_type z1_{};
    variable_type z2_{};
    variable_type z3_{};
    variable_type z4_{};
    variable_type z5_{};
    ///@}
};

/*!
 * \brief Class of solver using 4th order SDIRK (singly diagonally implicit
 * Runge-Kutta) formula in \cite Hairer1991.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using sdirk4_solver = embedded_solver<sdirk4_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
