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
 * \brief Definition of ros3w_formula class.
 */
#pragma once

#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/formula_base.h"
#include "num_collect/ode/runge_kutta/impl/rosenbrock_helper.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of ROS3w formula in \cite Rang2005 for Rosenbrock method.
 *
 * \tparam Problem Type of problem.
 */
template <typename Problem>
class ros3w_formula : public formula_base<ros3w_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<ros3w_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 3;

    //! Order of this formula.
    static constexpr index_type order = 3;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 2;

    /*!
     * \name Coefficients in Rosenbrock method.
     *
     * - `a` is coefficients of intermidiate variables in calculation of
     *   intermidiate variables.
     * - `b` is coefficients of time in calculation of intermidiate variables.
     * - `c` is coefficients of intermidiate variables in calculation of
     *   estimates of next variables.
     * - `g` is coefficients of intermidiate variables in calculation of
     *   intermidiate variables.
     */
    ///@{
    //! Coefficient in Rosenbrock method.
    static constexpr scalar_type a21 = coeff(6.666666666666666e-01);
    static constexpr scalar_type a31 = coeff(6.666666666666666e-01);
    static constexpr scalar_type a32 = coeff(0);

    static constexpr scalar_type b1 = coeff(0);
    static constexpr scalar_type b2 = a21;
    static constexpr scalar_type b3 = a31 + a32;

    static constexpr scalar_type g21 = coeff(3.635068368900681e-01);
    static constexpr scalar_type g31 = coeff(-8.996866791992636e-01);
    static constexpr scalar_type g32 = coeff(-1.537997822626885e-01);
    static constexpr scalar_type g = coeff(4.358665215084590e-01);

    static constexpr scalar_type c1 = coeff(2.500000000000000e-01);
    static constexpr scalar_type c2 = coeff(2.500000000000000e-01);
    static constexpr scalar_type c3 = coeff(5.000000000000000e-01);

    static constexpr scalar_type cw1 = coeff(7.467047032740110e-01);
    static constexpr scalar_type cw2 = coeff(1.144064078371002e-01);
    static constexpr scalar_type cw3 = coeff(1.388888888888889e-01);

    static constexpr scalar_type ce1 = c1 - cw1;
    static constexpr scalar_type ce2 = c2 - cw2;
    static constexpr scalar_type ce3 = c3 - cw3;
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
        problem().evaluate_on(time, current, true);
        jacobian_ = problem().jacobian();
        lu_solver_.compute(step_size * g * jacobian_);

        k1_ = lu_solver_.solve(problem().diff_coeff());

        problem().evaluate_on(
            time + b2 * step_size, current + step_size * (a21 * k1_));
        k2_ = lu_solver_.solve(
            problem().diff_coeff() + step_size * jacobian_ * (g21 * k1_));

        problem().evaluate_on(time + b3 * step_size,
            current + step_size * (a31 * k1_ + a32 * k2_));
        k3_ = lu_solver_.solve(problem().diff_coeff() +
            step_size * jacobian_ * (g31 * k1_ + g32 * k2_));

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

    //! Jacobian.
    jacobian_type jacobian_{};

    //! Solver of LU decomposition.
    impl::rosenbrock_helper<jacobian_type> lu_solver_{};
};

/*!
 * \brief Class of solver using ROS3w formula in \cite Rang2005.
 *
 * \tparam Problem Type of problem.
 */
template <typename Problem>
using ros3w_solver = embedded_solver<ros3w_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
