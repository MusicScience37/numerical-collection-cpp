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
 * \brief Test of ark54_esdirk_formula class.
 */
#include "num_collect/ode/runge_kutta/ark54_esdirk_formula.h"

#include <cmath>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/external_force_vibration_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::ark54_esdirk_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::ark54_esdirk_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 8);
        STATIC_REQUIRE(formula_type::order == 5);
        STATIC_REQUIRE(formula_type::lesser_order == 4);

        CHECK_THAT(formula_type::a21 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b2));
        CHECK_THAT(formula_type::a31 + formula_type::a32 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b3));
        CHECK_THAT(formula_type::a41 + formula_type::a43 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b4));
        CHECK_THAT(formula_type::a51 + formula_type::a53 + formula_type::a54 +
                formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b5));
        CHECK_THAT(formula_type::a61 + formula_type::a63 + formula_type::a64 +
                formula_type::a65 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b6));
        CHECK_THAT(formula_type::a71 + formula_type::a73 + formula_type::a74 +
                formula_type::a75 + formula_type::a76 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b7));
        CHECK_THAT(formula_type::a81 + formula_type::a84 + formula_type::a85 +
                formula_type::a86 + formula_type::a87 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b8));
        CHECK_THAT(formula_type::c1 + formula_type::c4 + formula_type::c5 +
                formula_type::c6 + formula_type::c7 + formula_type::c8,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::cw1 + formula_type::cw4 + formula_type::cw5 +
                formula_type::cw6 + formula_type::cw7 + formula_type::cw8,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::ce1 + formula_type::ce4 + formula_type::ce5 +
                formula_type::ce6 + formula_type::ce7 + formula_type::ce8,
            Catch::Matchers::WithinAbs(0.0, 1e-10));  // NOLINT
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

    SECTION("step_embedded") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double prev_var = 1.0;
        double next_var = 0.0;
        double error = 0.0;
        formula.step_embedded(time, step_size, prev_var, next_var, error);

        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference_and_error(
            next_var, error, reference);
    }
}

TEST_CASE(
    "num_collect::ode::runge_kutta::ark54_esdirk_solver<num_prob_collect::ode::"
    "exponential_problem>") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type =
        num_collect::ode::runge_kutta::ark54_esdirk_solver<problem_type>;

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
    "num_collect::ode::runge_kutta::ark54_esdirk_solver<num_prob_collect::ode::"
    "spring_movement_problem>") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::runge_kutta::ark54_esdirk_solver<problem_type>;

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

TEST_CASE(
    "num_collect::ode::runge_kutta::ark54_esdirk_solver<num_prob_collect::ode::"
    "external_force_vibration_problem>") {
    using problem_type =
        num_prob_collect::ode::external_force_vibration_problem;
    using solver_type =
        num_collect::ode::runge_kutta::ark54_esdirk_solver<problem_type>;

    SECTION("solve_till") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(-1.0, 0.0);
        solver.init(init_time, init_var);

        constexpr double duration = 2.345;
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
