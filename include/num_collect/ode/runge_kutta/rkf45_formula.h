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
 * \brief Definition of rkf45_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of Runge-Kutta-Fehlberg 45 formula.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
class rkf45_formula : public formula_base<rkf45_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<rkf45_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 6;

    //! Order of this formula.
    static constexpr index_type order = 5;

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::runge_kutta::rkf45_formula");

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
    static constexpr scalar_type a21 = coeff(1, 4);
    static constexpr scalar_type a31 = coeff(3, 32);
    static constexpr scalar_type a32 = coeff(9, 32);
    static constexpr scalar_type a41 = coeff(1932, 2197);
    static constexpr scalar_type a42 = coeff(-7200, 2197);
    static constexpr scalar_type a43 = coeff(7296, 2197);
    static constexpr scalar_type a51 = coeff(439, 216);
    static constexpr scalar_type a52 = coeff(-8);
    static constexpr scalar_type a53 = coeff(3680, 513);
    static constexpr scalar_type a54 = coeff(-845, 4104);
    static constexpr scalar_type a61 = coeff(-8, 27);
    static constexpr scalar_type a62 = coeff(2);
    static constexpr scalar_type a63 = coeff(-3544, 2565);
    static constexpr scalar_type a64 = coeff(1859, 4104);
    static constexpr scalar_type a65 = coeff(-11, 40);

    static constexpr scalar_type b2 = coeff(1, 4);
    static constexpr scalar_type b3 = coeff(3, 8);
    static constexpr scalar_type b4 = coeff(12, 13);
    static constexpr scalar_type b5 = coeff(1);
    static constexpr scalar_type b6 = coeff(1, 2);

    static constexpr scalar_type c1 = coeff(16, 135);
    static constexpr scalar_type c3 = coeff(6656, 12825);
    static constexpr scalar_type c4 = coeff(28561, 56430);
    static constexpr scalar_type c5 = coeff(-9, 50);
    static constexpr scalar_type c6 = coeff(2, 55);

    static constexpr scalar_type cw1 = coeff(25, 216);
    static constexpr scalar_type cw3 = coeff(1408, 2565);
    static constexpr scalar_type cw4 = coeff(2197, 4104);
    static constexpr scalar_type cw5 = coeff(-1, 5);

    static constexpr scalar_type ce1 = c1 - cw1;
    static constexpr scalar_type ce3 = c3 - cw3;
    static constexpr scalar_type ce4 = c4 - cw4;
    static constexpr scalar_type ce5 = c5 - cw5;
    static constexpr scalar_type ce6 = c6;
    ///@}

    //! \copydoc ode::formula_base::step
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
        problem().evaluate_on(time, current);
        k1_ = problem().diff_coeff();

        problem().evaluate_on(
            time + b2 * step_size, current + step_size * a21 * k1_);
        k2_ = problem().diff_coeff();

        problem().evaluate_on(time + b3 * step_size,
            current + step_size * (a31 * k1_ + a32 * k2_));
        k3_ = problem().diff_coeff();

        problem().evaluate_on(time + b4 * step_size,
            current + step_size * (a41 * k1_ + a42 * k2_ + a43 * k3_));
        k4_ = problem().diff_coeff();

        problem().evaluate_on(time + b5 * step_size,
            current +
                step_size * (a51 * k1_ + a52 * k2_ + a53 * k3_ + a54 * k4_));
        k5_ = problem().diff_coeff();

        problem().evaluate_on(time + b6 * step_size,
            current +
                step_size *
                    (a61 * k1_ + a62 * k2_ + a63 * k3_ + a64 * k4_ +
                        a65 * k5_));
        k6_ = problem().diff_coeff();

        estimate = current +
            step_size * (c1 * k1_ + c3 * k3_ + c4 * k4_ + c5 * k5_ + c6 * k6_);
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
    ///@}
};

/*!
 * \brief Class of solver using Runge-Kutta-Fehlberg 45 formula.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using rkf45_solver = embedded_solver<rkf45_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
