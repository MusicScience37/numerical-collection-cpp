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
 * \brief Test of lu_rosenbrock_equation_solver class.
 */
#include "num_collect/ode/rosenbrock/lu_rosenbrock_equation_solver.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "eigen_approx.h"
#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"  // IWYU pragma: keep
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver<
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
        const Eigen::Vector2d variable = Eigen::Vector2d(1.0, 0.0);
        constexpr double step_size = 0.1;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        Eigen::Matrix2d jacobian{};
        Eigen::Vector2d jacobian_col{};
        solver.apply_jacobian(Eigen::Vector2d{{1.0, 0.0}}, jacobian_col);
        jacobian.col(0) = jacobian_col;
        solver.apply_jacobian(Eigen::Vector2d{{0.0, 1.0}}, jacobian_col);
        jacobian.col(1) = jacobian_col;
        CHECK_THAT(jacobian, eigen_approx(problem.jacobian()));
    }

    SECTION("solve an equation") {
        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        const Eigen::Vector2d variable = Eigen::Vector2d(1.0, 0.0);
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        const Eigen::Vector2d expected_result = Eigen::Vector2d(0.123, -0.234);
        const Eigen::Vector2d rhs = expected_result -
            step_size * inverted_jacobian_coeff * problem.jacobian() *
                expected_result;
        Eigen::Vector2d result;
        solver.solve(rhs, result);

        CHECK_THAT(result, eigen_approx(expected_result));
    }
}
