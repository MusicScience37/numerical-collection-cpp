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
 * \brief Test of scalar_rosenbrock_equation_solver class.
 */
#include "num_collect/ode/rosenbrock/scalar_rosenbrock_equation_solver.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"  // IWYU pragma: keep
#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE("") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type =
        num_collect::ode::rosenbrock::scalar_rosenbrock_equation_solver<
            problem_type>;

    SECTION("check concept") {
        STATIC_REQUIRE(num_collect::ode::concepts::rosenbrock_equation_solver<
            solver_type>);
    }

    SECTION("update Jacobian") {
        solver_type solver;

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 0.1;
        constexpr double inverted_jacobian_coeff = 0.1;

        solver.init(problem, variable, inverted_jacobian_coeff);
        solver.update_jacobian(time, step_size, variable);

        CHECK_THAT(
            solver.jacobian(), Catch::Matchers::WithinRel(problem.jacobian()));
    }

    SECTION("try to update Jacobian before initialization") {
        solver_type solver;

        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 0.1;

        CHECK_THROWS(solver.update_jacobian(time, step_size, variable));
    }

    SECTION("invalid condition for inversion") {
        solver_type solver;

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 2.0;
        constexpr double inverted_jacobian_coeff = 0.5;

        solver.init(problem, variable, inverted_jacobian_coeff);
        CHECK_THROWS(solver.update_jacobian(time, step_size, variable));
    }

    SECTION("solve an equation") {
        solver_type solver;

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 0.01;
        constexpr double inverted_jacobian_coeff = 0.2;

        solver.init(problem, variable, inverted_jacobian_coeff);
        solver.update_jacobian(time, step_size, variable);

        constexpr double expected_result = 0.123;
        const double rhs = expected_result -
            step_size * inverted_jacobian_coeff * problem.jacobian() *
                expected_result;

        double result{0.0};
        solver.solve(rhs, result);

        CHECK_THAT(result, Catch::Matchers::WithinRel(expected_result));
    }
}
