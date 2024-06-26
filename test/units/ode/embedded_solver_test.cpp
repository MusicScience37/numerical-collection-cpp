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
 * \brief Test of embedded_solver class.
 */
#include "num_collect/ode/embedded_solver.h"

#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "eigen_approx.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE(
    "num_collect::ode::embedded_solver<num_collect::ode::runge_"
    "kutta::rkf45_formula<num_prob_collect::ode::exponential_problem>>") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::rkf45_formula<problem_type>;
    using solver_type = num_collect::ode::embedded_solver<formula_type>;

    SECTION("initialize") {
        auto solver = solver_type(problem_type());

        constexpr double step_size = 1e-4;
        REQUIRE_NOTHROW(solver.step_size(step_size));
        constexpr double init_time = 1.234;
        constexpr double init_var = 1.0;
        REQUIRE_NOTHROW(solver.init(init_time, init_var));

        REQUIRE_THAT(solver.time(), Catch::Matchers::WithinRel(init_time));
        REQUIRE_THAT(solver.variable(), Catch::Matchers::WithinRel(init_var));
        REQUIRE_THAT(solver.step_size(), Catch::Matchers::WithinRel(step_size));
        REQUIRE(solver.steps() == 0);
    }

    SECTION("step") {
        auto solver = solver_type(problem_type());

        constexpr double step_size = 1e-4;
        REQUIRE_NOTHROW(solver.step_size(step_size));
        constexpr double init_time = 1.234;
        constexpr double init_var = 1.0;
        solver.init(init_time, init_var);

        REQUIRE_NOTHROW(solver.step());

        REQUIRE_THAT(
            solver.time(), Catch::Matchers::WithinRel(init_time + step_size));
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        REQUIRE(solver.step_size() != step_size);
        REQUIRE(solver.steps() == 1);
    }

    SECTION("solve_till") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 1.234;
        constexpr double init_var = 1.0;
        solver.init(init_time, init_var);

        constexpr double duration = 2.345;
        constexpr double end_time = init_time + duration;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        REQUIRE_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const double reference = std::exp(duration);
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        REQUIRE(solver.steps() > 1);
    }
}

TEST_CASE(
    "num_collect::ode::embedded_solver<num_collect::ode::runge_"
    "kutta::rkf45_formula<num_prob_collect::ode::spring_movement_problem>>") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using formula_type =
        num_collect::ode::runge_kutta::rkf45_formula<problem_type>;
    using solver_type = num_collect::ode::embedded_solver<formula_type>;

    SECTION("initialize") {
        auto solver = solver_type(problem_type());

        constexpr double step_size = 1e-4;
        REQUIRE_NOTHROW(solver.step_size(step_size));
        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        REQUIRE_NOTHROW(solver.init(init_time, init_var));

        REQUIRE_THAT(solver.time(), Catch::Matchers::WithinRel(init_time));
        REQUIRE_THAT(solver.variable(), eigen_approx(init_var));
        REQUIRE_THAT(solver.step_size(), Catch::Matchers::WithinRel(step_size));
        REQUIRE(solver.steps() == 0);
    }

    SECTION("step") {
        auto solver = solver_type(problem_type());

        constexpr double step_size = 1e-4;
        REQUIRE_NOTHROW(solver.step_size(step_size));
        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        solver.init(init_time, init_var);

        REQUIRE_NOTHROW(solver.step());

        REQUIRE_THAT(solver.time(), Catch::Matchers::WithinRel(step_size));
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        REQUIRE(solver.step_size() != step_size);
        REQUIRE(solver.steps() == 1);
    }

    SECTION("solve_till") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        solver.init(init_time, init_var);

        constexpr double duration = 2.345;
        constexpr double end_time = init_time + duration;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        REQUIRE_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(end_time), std::sin(end_time));
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        REQUIRE(solver.steps() > 1);
    }
}
