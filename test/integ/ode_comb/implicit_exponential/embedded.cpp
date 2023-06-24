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
#include "comparison_approvals.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/ode/rosenbrock/ros34pw3_formula.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_prob_collect/ode/implicit_exponential_problem.h"

// NOLINTNEXTLINE
TEMPLATE_PRODUCT_TEST_CASE(
    "implicit_exponential_problem with embedded formulas", "",
    (
        // embedded Rosenbrock method.
        num_collect::ode::rosenbrock::rodasp_solver,
        num_collect::ode::rosenbrock::rodaspr_solver,
        num_collect::ode::rosenbrock::ros34pw3_solver,
        num_collect::ode::rosenbrock::ros3w_solver),
    (num_prob_collect::ode::implicit_exponential_problem)) {
    using problem_type = num_prob_collect::ode::implicit_exponential_problem;
    using solver_type = TestType;

    STATIC_REQUIRE(
        std::is_same_v<typename TestType::problem_type, problem_type>);

    SECTION("solve") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        constexpr double finish_time = 3.0;
        constexpr num_collect::index_type num_time_samples = 10;

        constexpr double init_var = 1.0;
        solver.init(init_time, init_var);

        solve_and_check_with_reference(solver, init_time, finish_time,
            num_time_samples, [](double time) { return std::exp(time); });
    }
}
