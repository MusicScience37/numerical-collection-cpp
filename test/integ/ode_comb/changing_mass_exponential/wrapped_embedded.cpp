/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Combinatorial test of ODE solvers.
 */
#include <type_traits>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../solve_and_check.h"
#include "num_collect/ode/implicit_problem_wrapper.h"
#include "num_collect/ode/runge_kutta/ark43_erk_formula.h"
#include "num_collect/ode/runge_kutta/dopri5_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_prob_collect/ode/changing_mass_exponential_problem.h"

using wrapped_changing_mass_exponential_problem =
    num_collect::ode::implicit_problem_wrapper<
        num_prob_collect::ode::changing_mass_exponential_problem>;

// NOLINTNEXTLINE
TEMPLATE_PRODUCT_TEST_CASE(
    "changing_mass_exponential_problem with embedded formulas with "
    "implicit_problem_wrapper",
    "",
    (
        // embedded Runge-Kutta method.
        num_collect::ode::runge_kutta::ark43_erk_solver,
        num_collect::ode::runge_kutta::dopri5_solver,
        num_collect::ode::runge_kutta::rkf45_solver),
    (wrapped_changing_mass_exponential_problem)) {
    using problem_type =
        num_prob_collect::ode::changing_mass_exponential_problem;
    using solver_type = TestType;

    SECTION("solve") {
        auto solver = solver_type(
            num_collect::ode::wrap_implicit_problem(problem_type()));

        constexpr double init_time = 0.0;
        constexpr double finish_time = 3.0;
        constexpr num_collect::index_type num_time_samples = 10;

        constexpr double init_var = 1.0;
        solver.init(init_time, init_var);

        solve_and_check_with_reference(solver, init_time, finish_time,
            num_time_samples, [](double time) { return std::exp(time); });
    }
}
