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
 * \brief Definition of dopri5_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of DOPRI5 formula.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
class dopri5_formula : public formula_base<dopri5_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<dopri5_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 7;

    //! Order of this formula.
    static constexpr index_type order = 5;

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::runge_kutta::dopri5_formula");

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 4;

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
    static constexpr scalar_type a21 = coeff(1, 5);
    static constexpr scalar_type a31 = coeff(3, 40);
    static constexpr scalar_type a32 = coeff(9, 40);
    static constexpr scalar_type a41 = coeff(44, 45);
    static constexpr scalar_type a42 = coeff(-56, 15);
    static constexpr scalar_type a43 = coeff(32, 9);
    static constexpr scalar_type a51 = coeff(19372, 6561);
    static constexpr scalar_type a52 = coeff(-25360, 2187);
    static constexpr scalar_type a53 = coeff(64448, 6561);
    static constexpr scalar_type a54 = coeff(-212, 729);
    static constexpr scalar_type a61 = coeff(9017, 3168);
    static constexpr scalar_type a62 = coeff(-355, 33);
    static constexpr scalar_type a63 = coeff(46732, 5247);
    static constexpr scalar_type a64 = coeff(49, 176);
    static constexpr scalar_type a65 = coeff(-5103, 18656);
    static constexpr scalar_type a71 = coeff(35, 384);
    static constexpr scalar_type a72 = coeff(0);
    static constexpr scalar_type a73 = coeff(500, 1113);
    static constexpr scalar_type a74 = coeff(125, 192);
    static constexpr scalar_type a75 = coeff(-2187, 6784);
    static constexpr scalar_type a76 = coeff(11, 84);

    static constexpr scalar_type b2 = coeff(1, 5);
    static constexpr scalar_type b3 = coeff(3, 10);
    static constexpr scalar_type b4 = coeff(4, 5);
    static constexpr scalar_type b5 = coeff(8, 9);
    static constexpr scalar_type b6 = coeff(1);
    static constexpr scalar_type b7 = coeff(1);

    static constexpr scalar_type c1 = a71;
    // c2 = 0
    static constexpr scalar_type c3 = a73;
    static constexpr scalar_type c4 = a74;
    static constexpr scalar_type c5 = a75;
    static constexpr scalar_type c6 = a76;
    // c7 = 0

    static constexpr scalar_type cw1 = coeff(5179, 57600);
    // c22 = 0
    static constexpr scalar_type cw3 = coeff(7571, 16695);
    static constexpr scalar_type cw4 = coeff(393, 640);
    static constexpr scalar_type cw5 = coeff(-92097, 339200);
    static constexpr scalar_type cw6 = coeff(187, 2100);
    static constexpr scalar_type cw7 = coeff(1, 40);

    static constexpr scalar_type ce1 = c1 - cw1;
    // ce2 = 0
    static constexpr scalar_type ce3 = c3 - cw3;
    static constexpr scalar_type ce4 = c4 - cw4;
    static constexpr scalar_type ce5 = c5 - cw5;
    static constexpr scalar_type ce6 = c6 - cw6;
    static constexpr scalar_type ce7 = -cw7;
    ///@}
};

}  // namespace num_collect::ode::runge_kutta
