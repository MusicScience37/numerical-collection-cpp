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
 * \brief Test of rk4_formula class.
 */
#include "num_collect/ode/runge_kutta/rk4_formula.h"

#include <cmath>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/external_force_vibration_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::rk4_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::rk4_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 4);
        STATIC_REQUIRE(formula_type::order == 4);

        CHECK(0.0 == formula_type::b1);
        CHECK_THAT(
            formula_type::a21, Catch::Matchers::WithinRel(formula_type::b2));
        CHECK_THAT(
            formula_type::a32, Catch::Matchers::WithinRel(formula_type::b3));
        CHECK_THAT(
            formula_type::a43, Catch::Matchers::WithinRel(formula_type::b4));
        CHECK_THAT(formula_type::c1 + formula_type::c2 + formula_type::c3 +
                formula_type::c4,
            Catch::Matchers::WithinRel(1.0));
    }

    SECTION("initialize") {
        auto formula = formula_type(problem_type());
        (void)formula;
    }

    SECTION("step") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-4;
        constexpr double prev_var = 1.0;
        double next_var = 0.0;
        formula.step(time, step_size, prev_var, next_var);

        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(next_var, reference);
    }
}

TEST_CASE(
    "num_collect::ode::runge_kutta::rk4_solver<num_prob_collect::ode::"
    "exponential_problem>") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type = num_collect::ode::runge_kutta::rk4_solver<problem_type>;

    SECTION("solve_till") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 1.234;
        constexpr double init_var = 1.0;
        solver.init(init_time, init_var);

        constexpr double duration = 0.1;
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
    "num_collect::ode::runge_kutta::rk4_solver<num_prob_collect::ode::spring_"
    "movement_problem>") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type = num_collect::ode::runge_kutta::rk4_solver<problem_type>;

    SECTION("solve_till") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        solver.init(init_time, init_var);

        constexpr double duration = 0.1;
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

TEST_CASE(
    "num_collect::ode::runge_kutta::rk4_solver<num_prob_collect::ode::external_"
    "force_vibration_problem>") {
    using problem_type =
        num_prob_collect::ode::external_force_vibration_problem;
    using solver_type = num_collect::ode::runge_kutta::rk4_solver<problem_type>;

    SECTION("solve_till") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(-1.0, 0.0);
        solver.init(init_time, init_var);

        constexpr double duration = 0.1;
        constexpr double end_time = init_time + duration;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        REQUIRE_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference =
            Eigen::Vector2d(-std::cos(end_time), -std::sin(end_time));
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        REQUIRE(solver.steps() > 1);
    }
}
