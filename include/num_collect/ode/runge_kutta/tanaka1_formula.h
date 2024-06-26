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
 * \brief Definition of tanaka1_formula class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/concepts/slope_equation_solver.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/inexact_newton_slope_equation_solver.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of Tanaka Formula 1.
 *
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <concepts::problem Problem,
    concepts::slope_equation_solver FormulaSolver =
        inexact_newton_slope_equation_solver<Problem>>
class tanaka1_formula
    : public implicit_formula_base<tanaka1_formula<Problem, FormulaSolver>,
          Problem, FormulaSolver> {
public:
    //! Type of this class.
    using this_type = tanaka1_formula<Problem, FormulaSolver>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<tanaka1_formula<Problem, FormulaSolver>, Problem,
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
    static constexpr index_type stages = 2;

    //! Order of this formula.
    static constexpr index_type order = 3;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 1;

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::runge_kutta::tanaka1_formula");

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
    static constexpr scalar_type a11 = coeff(13, 20);
    static constexpr scalar_type a21 = coeff(-127, 180);
    static constexpr scalar_type a22 = coeff(13, 20);

    static constexpr scalar_type b1 = coeff(13, 20);
    static constexpr scalar_type b2 = coeff(-1, 18);

    static constexpr scalar_type c1 = coeff(100, 127);
    static constexpr scalar_type c2 = coeff(27, 127);

    static constexpr scalar_type cw1 = coeff(1);

    static constexpr scalar_type ce1 = c1 - cw1;
    static constexpr scalar_type ce2 = c2;
    ///@}

    //! \copydoc runge_kutta::implicit_formula_base::step
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
        formula_solver().update_jacobian(
            problem(), time + b1 * step_size, step_size, current, a11);
        k1_ = problem().diff_coeff();
        formula_solver().init(k1_);
        formula_solver().solve();

        formula_solver().update_jacobian(problem(), time + b2 * step_size,
            step_size, current + step_size * a21 * k1_, a22);
        k2_ = problem().diff_coeff();
        formula_solver().init(k2_);
        formula_solver().solve();

        estimate = current + step_size * (c1 * k1_ + c2 * k2_);
        error = step_size * (ce1 * k1_ + ce2 * k2_);
    }

private:
    /*!
     * \name Intermediate variables.
     */
    ///@{
    //! Intermediate variable.
    variable_type k1_{};
    variable_type k2_{};
    ///@}
};

/*!
 * \brief Class of solver using Tanaka Formula 1.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using tanaka1_solver = embedded_solver<tanaka1_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
