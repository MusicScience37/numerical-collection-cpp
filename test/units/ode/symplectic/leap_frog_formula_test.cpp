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
 * \brief Test of leap_frog_formula class.
 */
#include "num_collect/ode/symplectic/leap_frog_formula.h"

#include <cmath>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "num_prob_collect/ode/autonomous_external_force_vibration_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::symplectic::leap_frog_formula") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using formula_type =
        num_collect::ode::symplectic::leap_frog_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 3);
        STATIC_REQUIRE(formula_type::order == 2);
    }

    SECTION("initialize") {
        auto formula = formula_type(problem_type());
        (void)formula;
    }

    SECTION("step") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        const Eigen::Vector2d prev_var = Eigen::Vector2d(1.0, 0.0);
        Eigen::Vector2d next_var{{0.0, 0.0}};
        formula.step(time, step_size, prev_var, next_var);

        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(next_var, reference);
    }
}

TEST_CASE("num_collect::ode::symplectic::leap_frog_formula(invalid problem)") {
    using problem_type =
        num_prob_collect::ode::autonomous_external_force_vibration_problem;
    using formula_type =
        num_collect::ode::symplectic::leap_frog_formula<problem_type>;

    SECTION("step") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        const Eigen::Vector3d prev_var = Eigen::Vector3d{{1.0, 0.0, 0.0}};
        Eigen::Vector3d next_var{{0.0, 0.0, 0.0}};
        CHECK_THROWS(formula.step(time, step_size, prev_var, next_var));
    }
}

TEST_CASE(
    "num_collect::ode::symplectic::leap_frog_solver<num_prob_collect::ode::"
    "spring_movement_problem>") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::symplectic::leap_frog_solver<problem_type>;

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
