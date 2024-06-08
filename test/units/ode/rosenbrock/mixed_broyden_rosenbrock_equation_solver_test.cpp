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
 * \brief Test of mixed_broyden_rosenbrock_equation_solver class.
 */
#include "num_collect/ode/rosenbrock/mixed_broyden_rosenbrock_equation_solver.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "comparison_approvals.h"
#include "eigen_approx.h"
#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/lu_rosenbrock_equation_solver.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE(
    "num_collect::ode::rosenbrock::mixed_broyden_rosenbrock_equation_solver") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::rosenbrock::mixed_broyden_rosenbrock_equation_solver<
            problem_type>;
    using reference_solver_type =
        num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver<
            problem_type>;

    SECTION("check concept") {
        STATIC_REQUIRE(num_collect::ode::concepts::rosenbrock_equation_solver<
            solver_type>);
    }

    SECTION("first evaluation") {
        constexpr double inverted_jacobian_coeff = 0.1;
        solver_type solver{inverted_jacobian_coeff};
        reference_solver_type reference_solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        const Eigen::Vector2d variable{{1.0, 0.0}};
        constexpr double step_size = 0.1;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);
        reference_solver.evaluate_and_update_jacobian(
            problem, time, step_size, variable);

        SECTION("apply Jacobian") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.apply_jacobian(target, result);
            Eigen::Vector2d reference;
            reference_solver.apply_jacobian(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }

        SECTION("time derivative") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result = target;
            solver.add_time_derivative_term(
                step_size, inverted_jacobian_coeff, result);
            CHECK_THAT(result, eigen_approx(target));
        }

        SECTION("solve") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.solve(target, result);
            Eigen::Vector2d reference;
            reference_solver.solve(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }
    }

    SECTION("approx evaluation") {
        constexpr double inverted_jacobian_coeff = 0.1;
        solver_type solver{inverted_jacobian_coeff};
        reference_solver_type reference_solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        const Eigen::Vector2d variable{{1.0, 0.0}};
        constexpr double step_size = 1e-3;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);
        reference_solver.evaluate_and_update_jacobian(
            problem, time, step_size, variable);

        constexpr double time2 = step_size;
        const Eigen::Vector2d variable2{{1.0 - 1e-8, 1e-4}};
        constexpr double step_size2 = 2e-3;
        solver.evaluate_and_update_jacobian(
            problem, time2, step_size2, variable2);
        reference_solver.evaluate_and_update_jacobian(
            problem, time2, step_size2, variable2);

        SECTION("apply Jacobian") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.apply_jacobian(target, result);
            Eigen::Vector2d reference;
            reference_solver.apply_jacobian(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }

        SECTION("time derivative") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result = target;
            solver.add_time_derivative_term(
                step_size, inverted_jacobian_coeff, result);
            CHECK_THAT(result, eigen_approx(target));
        }

        SECTION("solve") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.solve(target, result);
            Eigen::Vector2d reference;
            reference_solver.solve(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }
    }

    SECTION("second evaluation with the same time") {
        constexpr double inverted_jacobian_coeff = 0.1;
        solver_type solver{inverted_jacobian_coeff};
        reference_solver_type reference_solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        const Eigen::Vector2d variable{{1.0, 0.0}};
        constexpr double step_size = 0.1;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);
        reference_solver.evaluate_and_update_jacobian(
            problem, time, step_size, variable);
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);
        reference_solver.evaluate_and_update_jacobian(
            problem, time, step_size, variable);

        SECTION("apply Jacobian") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.apply_jacobian(target, result);
            Eigen::Vector2d reference;
            reference_solver.apply_jacobian(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }

        SECTION("time derivative") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result = target;
            solver.add_time_derivative_term(
                step_size, inverted_jacobian_coeff, result);
            CHECK_THAT(result, eigen_approx(target));
        }

        SECTION("solve") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.solve(target, result);
            Eigen::Vector2d reference;
            reference_solver.solve(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }
    }

    SECTION("second evaluation with the same variable") {
        constexpr double inverted_jacobian_coeff = 0.1;
        solver_type solver{inverted_jacobian_coeff};
        reference_solver_type reference_solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        const Eigen::Vector2d variable{{1.0, 0.0}};
        constexpr double step_size = 0.1;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);
        reference_solver.evaluate_and_update_jacobian(
            problem, time, step_size, variable);
        constexpr double time2 = 0.1;
        solver.evaluate_and_update_jacobian(
            problem, time2, step_size, variable);
        reference_solver.evaluate_and_update_jacobian(
            problem, time2, step_size, variable);

        SECTION("apply Jacobian") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.apply_jacobian(target, result);
            Eigen::Vector2d reference;
            reference_solver.apply_jacobian(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }

        SECTION("time derivative") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result = target;
            solver.add_time_derivative_term(
                step_size, inverted_jacobian_coeff, result);
            CHECK_THAT(result, eigen_approx(target));
        }

        SECTION("solve") {
            const Eigen::Vector2d target{{1.0, 2.0}};
            Eigen::Vector2d result;
            solver.solve(target, result);
            Eigen::Vector2d reference;
            reference_solver.solve(target, reference);
            comparison_approvals::verify_with_reference(result, reference);
        }
    }
}
