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
 * \brief Definition of rk4_formula class.
 */
#pragma once

#include "num_collect/ode/runge_kutta/formula_base.h"
#include "num_collect/ode/runge_kutta/simple_solver.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of Runge-Kutta 4 formula (classic Runge-Kutta method).
 *
 * \tparam Problem Type of problem.
 */
template <typename Problem>
class rk4_formula : public formula_base<rk4_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<rk4_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 4;

    //! Order of this formula.
    static constexpr index_type order = 4;

    /*!
     * \name Coefficients in Butcher array.
     */
    ///@{
    //! Coefficnet in Butcher array.
    static constexpr scalar_type a21 = coeff(1, 2);
    static constexpr scalar_type a32 = coeff(1, 2);
    static constexpr scalar_type a43 = coeff(1);

    static constexpr scalar_type b1 = coeff(0);
    static constexpr scalar_type b2 = coeff(1, 2);
    static constexpr scalar_type b3 = coeff(1, 2);
    static constexpr scalar_type b4 = coeff(1);

    static constexpr scalar_type c1 = coeff(1, 6);
    static constexpr scalar_type c2 = coeff(1, 3);
    static constexpr scalar_type c3 = coeff(1, 3);
    static constexpr scalar_type c4 = coeff(1, 6);
    ///@}

    //! \copydoc runge_kutta::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        problem().evaluate_on(time, current);
        k1_ = problem().diff_coeff();

        problem().evaluate_on(
            time + b2 * step_size, current + step_size * a21 * k1_);
        k2_ = problem().diff_coeff();

        problem().evaluate_on(
            time + b3 * step_size, current + step_size * a32 * k2_);
        k3_ = problem().diff_coeff();

        problem().evaluate_on(
            time + step_size, current + step_size * a43 * k3_);
        k4_ = problem().diff_coeff();

        estimate =
            current + step_size * (c1 * k1_ + c2 * k2_ + c3 * k3_ + c4 * k4_);
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
    ///@}
};

/*!
 * \brief Class of solver using Runge-Kutta 4 formula (classic Runge-Kutta
 * method).
 *
 * \tparam Problem Type of problem.
 */
template <typename Problem>
using rk4_solver = simple_solver<rk4_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
