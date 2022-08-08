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

#include <cmath>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"  // IWYU pragma: keep
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/external_exponential_problem.h"

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
        constexpr double inverted_jacobian_coeff = 0.1;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 0.1;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        double jacobian{0.0};
        solver.apply_jacobian(1.0, jacobian);
        CHECK_THAT(jacobian, Catch::Matchers::WithinRel(problem.jacobian()));
    }

    SECTION("invalid condition for inversion") {
        constexpr double inverted_jacobian_coeff = 0.5;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 2.0;
        CHECK_THROWS(solver.evaluate_and_update_jacobian(
            problem, time, step_size, variable));
    }

    SECTION("solve an equation") {
        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        constexpr double expected_result = 0.123;
        const double rhs = expected_result -
            step_size * inverted_jacobian_coeff * problem.jacobian() *
                expected_result;

        double result{0.0};
        solver.solve(rhs, result);

        CHECK_THAT(result, Catch::Matchers::WithinRel(expected_result));
    }

    SECTION("time derivative") {
        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        double target = 0.0;
        constexpr double coeff = 1.0;
        solver.add_time_derivative_term(step_size, coeff, target);
        CHECK(target == 0.0);
    }

    SECTION("time derivative for non-autonomous system") {
        using problem_type =
            num_prob_collect::ode::external_exponential_problem;
        using solver_type =
            num_collect::ode::rosenbrock::scalar_rosenbrock_equation_solver<
                problem_type>;

        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 1.0;
        constexpr double variable = 1.0;
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        double target = 0.0;
        constexpr double coeff = 0.2;
        solver.add_time_derivative_term(step_size, coeff, target);
        CHECK_THAT(target,
            Catch::Matchers::WithinRel(step_size * coeff * std::exp(time)));
    }
}
