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
 * \brief Test of solving implicit_kaps_problem.
 */
#include "num_prob_collect/ode/implicit_kaps_problem.h"

#include <cmath>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/rosenbrock/lu_rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"

TEST_CASE("implicit_kaps_problem + lu_rosenbrock_equation_solver + RODASP") {
    using problem_type = num_prob_collect::ode::implicit_kaps_problem;
    using rosenbrock_equation_solver_type =
        num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver<
            problem_type>;
    using formula_type =
        num_collect::ode::rosenbrock::rodasp_formula<problem_type,
            rosenbrock_equation_solver_type>;
    using solver_type = num_collect::ode::embedded_solver<formula_type>;

    SECTION("epsilon = 1") {
        constexpr double epsilon = 1.0;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }

    SECTION("epsilon = 0.1") {
        constexpr double epsilon = 0.1;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }

    SECTION("epsilon = 0.0001") {
        constexpr double epsilon = 0.0001;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }

    SECTION("epsilon = 0 (index = 1") {
        constexpr double epsilon = 0.0;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }
}

TEST_CASE("implicit_kaps_problem + lu_rosenbrock_equation_solver + RODASPR") {
    using problem_type = num_prob_collect::ode::implicit_kaps_problem;
    using rosenbrock_equation_solver_type =
        num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver<
            problem_type>;
    using formula_type =
        num_collect::ode::rosenbrock::rodaspr_formula<problem_type,
            rosenbrock_equation_solver_type>;
    using solver_type = num_collect::ode::embedded_solver<formula_type>;

    SECTION("epsilon = 1") {
        constexpr double epsilon = 1.0;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }

    SECTION("epsilon = 0.1") {
        constexpr double epsilon = 0.1;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }

    SECTION("epsilon = 0.0001") {
        constexpr double epsilon = 0.0001;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }

    SECTION("epsilon = 0 (index = 1") {
        constexpr double epsilon = 0.0;
        auto solver = solver_type(problem_type(epsilon));

        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var{{1.0, 1.0}};
        solver.init(init_time, init_var);

        constexpr double end_time = 0.1;
        REQUIRE_NOTHROW(solver.solve_till(end_time));

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const Eigen::Vector2d reference{
            {std::exp(-2.0 * end_time), std::exp(-end_time)}};
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }
}
