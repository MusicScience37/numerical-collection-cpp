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
 * \brief Definition of implicit_euler_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/constants/one.h"  // IWYU pragma: keep
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/slope_equation_solver.h"  // IWYU pragma: keep
#include "num_collect/ode/inexact_newton_slope_equation_solver.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"
#include "num_collect/ode/simple_solver.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of implicit Euler method.
 *
 * \tparam Problem Type of problem.
 * \tparam FormulaSolver Type of solver of formula.
 */
template <concepts::differentiable_problem Problem,
    concepts::slope_equation_solver FormulaSolver =
        inexact_newton_slope_equation_solver<Problem>>
class implicit_euler_formula
    : public implicit_formula_base<
          implicit_euler_formula<Problem, FormulaSolver>, Problem,
          FormulaSolver> {
public:
    //! Type of this class.
    using this_type = implicit_euler_formula<Problem, FormulaSolver>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<implicit_euler_formula<Problem, FormulaSolver>,
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
    static constexpr index_type stages = 1;

    //! Order of this formula.
    static constexpr index_type order = 2;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::implicit_euler_formula");

    //! \copydoc runge_kutta::implicit_formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        formula_solver().update_jacobian(problem(), time + step_size, step_size,
            current, static_cast<scalar_type>(1));
        slope_ = problem().diff_coeff();
        formula_solver().init(slope_);
        formula_solver().solve();
        estimate = current + step_size * slope_;
    }

private:
    //! Slope.
    variable_type slope_{};
};

/*!
 * \brief Class of solver using implicit Euler method.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using implicit_euler_solver = simple_solver<implicit_euler_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
