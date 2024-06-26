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

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of DOPRI5 formula using coefficients in \cite Hairer1991.
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

    //! \copydoc ode::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        constexpr auto evaluations = evaluation_type{.diff_coeff = true};

        problem().evaluate_on(time, current, evaluations);
        k1_ = problem().diff_coeff();

        problem().evaluate_on(time + b2 * step_size,
            current + step_size * a21 * k1_, evaluations);
        k2_ = problem().diff_coeff();

        problem().evaluate_on(time + b3 * step_size,
            current + step_size * (a31 * k1_ + a32 * k2_), evaluations);
        k3_ = problem().diff_coeff();

        problem().evaluate_on(time + b4 * step_size,
            current + step_size * (a41 * k1_ + a42 * k2_ + a43 * k3_),
            evaluations);
        k4_ = problem().diff_coeff();

        problem().evaluate_on(time + b5 * step_size,
            current +
                step_size * (a51 * k1_ + a52 * k2_ + a53 * k3_ + a54 * k4_),
            evaluations);
        k5_ = problem().diff_coeff();

        problem().evaluate_on(time + b6 * step_size,
            current +
                step_size *
                    (a61 * k1_ + a62 * k2_ + a63 * k3_ + a64 * k4_ + a65 * k5_),
            evaluations);
        k6_ = problem().diff_coeff();

        estimate = current +
            step_size * (c1 * k1_ + c3 * k3_ + c4 * k4_ + c5 * k5_ + c6 * k6_);
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

        constexpr auto evaluations = evaluation_type{.diff_coeff = true};
        problem().evaluate_on(time + b7 * step_size, estimate, evaluations);
        k7_ = problem().diff_coeff();

        error = step_size *
            (ce1 * k1_ + ce3 * k3_ + ce4 * k4_ + ce5 * k5_ + ce6 * k6_ +
                ce7 * k7_);
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
    ///@}
};

/*!
 * \brief Class of solver using DOPRI5 formula with coefficients in
 * \cite Hairer1991.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using dopri5_solver = embedded_solver<dopri5_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
