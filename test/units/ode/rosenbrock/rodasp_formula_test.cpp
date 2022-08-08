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
 * \brief Test of rodasp_formula class.
 */
#include "num_collect/ode/rosenbrock/rodasp_formula.h"

#include <cmath>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_collect/ode/rosenbrock/scalar_rosenbrock_equation_solver.h"
#include "num_prob_collect/ode/autonomous_external_force_vibration_problem.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/external_force_vibration_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::rosenbrock::rodasp_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::rosenbrock::rodasp_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 6);
        STATIC_REQUIRE(formula_type::order == 4);

        CHECK(formula_type::b1 == 0.0);
        CHECK_THAT(
            formula_type::a21, Catch::Matchers::WithinRel(formula_type::b2));
        CHECK_THAT(formula_type::a31 + formula_type::a32,
            Catch::Matchers::WithinRel(formula_type::b3));
        CHECK_THAT(formula_type::a41 + formula_type::a42 + formula_type::a43,
            Catch::Matchers::WithinRel(formula_type::b4));
        CHECK_THAT(formula_type::a51 + formula_type::a52 + formula_type::a53 +
                formula_type::a54,
            Catch::Matchers::WithinRel(formula_type::b5));
        CHECK_THAT(formula_type::a61 + formula_type::a62 + formula_type::a63 +
                formula_type::a64 + formula_type::a65,
            Catch::Matchers::WithinRel(formula_type::b6));
        CHECK_THAT(formula_type::c1 + formula_type::c2 + formula_type::c3 +
                formula_type::c4 + formula_type::c5 + formula_type::c6,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::cw1 + formula_type::cw2 + formula_type::cw3 +
                formula_type::cw4 + formula_type::cw5,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::ce1 + formula_type::ce2 + formula_type::ce3 +
                formula_type::ce4 + formula_type::ce5 + formula_type::ce6,
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
    "num_collect::ode::rosenbrock::rodasp_solver<num_prob_collect::ode::"
    "exponential_problem>") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type =
        num_collect::ode::rosenbrock::rodasp_solver<problem_type>;

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
    "num_collect::ode::rosenbrock::rodasp_solver<num_prob_collect::ode::"
    "spring_movement_problem>") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::rosenbrock::rodasp_solver<problem_type>;

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
    "num_collect::ode::rosenbrock::rodasp_solver<num_prob_collect::ode::"
    "external_force_vibration_problem>") {
    using problem_type =
        num_prob_collect::ode::external_force_vibration_problem;
    using solver_type =
        num_collect::ode::rosenbrock::rodasp_solver<problem_type>;

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

TEST_CASE(
    "num_collect::ode::rosenbrock::rodasp_solver<num_prob_collect::ode::"
    "autonomous_external_force_vibration_problem>") {
    using problem_type =
        num_prob_collect::ode::autonomous_external_force_vibration_problem;
    using solver_type =
        num_collect::ode::rosenbrock::rodasp_solver<problem_type>;

    SECTION("solve_till") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        const Eigen::Vector3d init_var = Eigen::Vector3d(-1.0, 0.0, init_time);
        solver.init(init_time, init_var);

        constexpr double duration = 2.345;
        constexpr double end_time = init_time + duration;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        REQUIRE_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector3d reference =
            Eigen::Vector3d(-std::cos(end_time), -std::sin(end_time), end_time);
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        REQUIRE(solver.steps() > 1);
    }
}
