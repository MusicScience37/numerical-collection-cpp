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
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::rk4_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::rk4_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 4);
        STATIC_REQUIRE(formula_type::order == 4);
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
        constexpr double tol = 1e-12;
        REQUIRE_THAT(next_var, Catch::Matchers::WithinRel(reference, tol));
    }
}

TEST_CASE("num_collect::ode::runge_kutta::rk4_solver") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type = num_collect::ode::runge_kutta::rk4_solver<problem_type>;

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
        constexpr double tol = 1e-12;
        REQUIRE_THAT(
            solver.variable(), Catch::Matchers::WithinRel(reference, tol));
        REQUIRE_THAT(solver.step_size(), Catch::Matchers::WithinRel(step_size));
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
        constexpr double tol = 1e-10;
        REQUIRE_THAT(
            solver.variable(), Catch::Matchers::WithinRel(reference, tol));
        REQUIRE(solver.steps() > 1);
    }

    SECTION("solve_till with logging") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 1.234;
        constexpr double init_var = 1.0;
        solver.init(init_time, init_var);

        constexpr double duration = 2.345;
        constexpr double end_time = init_time + duration;
        std::ostringstream stream;
        REQUIRE_NOTHROW(solver.solve_till(end_time, stream));

        REQUIRE_THAT(stream.str(), Catch::Matchers::Contains("Steps"));
        REQUIRE_THAT(stream.str(), Catch::Matchers::Contains("Time"));
        REQUIRE_THAT(stream.str(), Catch::Matchers::Contains("StepSize"));
    }
}
