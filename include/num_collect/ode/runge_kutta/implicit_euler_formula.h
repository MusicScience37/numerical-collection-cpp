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

#include "num_collect/constants/one.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/runge_kutta/implicit_formula_base.h"
#include "num_collect/ode/runge_kutta/semi_implicit_formula_solver.h"
#include "num_collect/ode/simple_solver.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of implicit Euler method.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem, typename StrategyTag>
class implicit_euler_formula
    : public implicit_formula_base<implicit_euler_formula<Problem, StrategyTag>,
          Problem, semi_implicit_formula_solver<Problem, StrategyTag>> {
public:
    //! Type of this class.
    using this_type = implicit_euler_formula<Problem, StrategyTag>;

    //! Type of base class.
    using base_type =
        implicit_formula_base<implicit_euler_formula<Problem, StrategyTag>,
            Problem, semi_implicit_formula_solver<Problem, StrategyTag>>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;
    using base_type::formula_solver;
    using base_type::tol_residual_norm;

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
        formula_solver().tol_residual_norm(
            tol_residual_norm(current, step_size));
        formula_solver().solve(
            time, step_size, current, constants::one<scalar_type>);
        estimate = current + step_size * formula_solver().k();
    }
};

/*!
 * \brief Class of solver using implicit Euler method.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using implicit_euler_solver = simple_solver<implicit_euler_formula<Problem,
    implicit_formula_solver_strategies::modified_newton_raphson_tag>>;

}  // namespace num_collect::ode::runge_kutta
