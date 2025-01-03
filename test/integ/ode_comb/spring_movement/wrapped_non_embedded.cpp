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
#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../solve_and_check.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/avf/avf2_formula.h"
#include "num_collect/ode/avf/avf3_formula.h"
#include "num_collect/ode/avf/avf4_formula.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

// NOLINTNEXTLINE
TEMPLATE_PRODUCT_TEST_CASE(
    "spring_movement_problem with non-embedded formulas with automatic step "
    "sizes",
    "",
    (num_collect::ode::runge_kutta::rk4_auto_solver,
        // AVF methods.
        num_collect::ode::avf::avf2_auto_solver,
        num_collect::ode::avf::avf3_auto_solver,
        num_collect::ode::avf::avf4_auto_solver),
    (num_prob_collect::ode::spring_movement_problem)) {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type = TestType;

    STATIC_REQUIRE(
        std::is_same_v<typename TestType::problem_type, problem_type>);

    SECTION("solve") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        constexpr double finish_time = 3.0;
        constexpr num_collect::index_type num_time_samples = 10;

        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        solver.init(init_time, init_var);

        constexpr double tolerance = 1e-4;
        solver.tolerances(num_collect::ode::error_tolerances<Eigen::Vector2d>()
                .tol_rel_error(tolerance)
                .tol_abs_error(tolerance));

        solve_and_check_with_reference(
            solver, init_time, finish_time, num_time_samples,
            [](double time) {
                return Eigen::Vector2d(std::cos(time), std::sin(time));
            },
            "auto");
    }
}
