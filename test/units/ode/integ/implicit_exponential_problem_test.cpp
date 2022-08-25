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
 * \brief Test of solving implicit_exponential_problem.
 */
#include "num_prob_collect/ode/implicit_exponential_problem.h"

#include <cmath>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/scalar_rosenbrock_equation_solver.h"

TEST_CASE(
    "num_collect::ode::rosenbrock::rodasp_solver<num_prob_collect::ode::"
    "implicit_exponential_problem>") {
    using problem_type = num_prob_collect::ode::implicit_exponential_problem;
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

        CHECK_THAT(solver.time(), Catch::Matchers::WithinRel(end_time));
        const double reference = std::exp(duration);
        comparison_approvals::verify_with_reference(
            solver.variable(), reference);
        CHECK(solver.steps() > 1);
    }
}
