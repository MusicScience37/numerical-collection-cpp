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
 * \brief Test of bicgstab_rosenbrock_equation_solver class.
 */
#include "num_collect/ode/rosenbrock/bicgstab_rosenbrock_equation_solver.h"

#include <cmath>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/util/vector.h"
#include "num_prob_collect/ode/external_force_vibration_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::rosenbrock::bicgstab_rosenbrock_equation_solver") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::rosenbrock::bicgstab_rosenbrock_equation_solver<
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
        problem.evaluate_on(time, variable,
            num_collect::ode::evaluation_type{.jacobian = true});
        comparison_approvals::verify_with_reference(
            jacobian, problem.jacobian());
    }

    SECTION("solve an equation") {
        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};
        solver.tolerances(
            num_collect::ode::error_tolerances<Eigen::Vector2d>());

        problem_type problem;
        constexpr double time = 0.0;
        const Eigen::Vector2d variable = Eigen::Vector2d(1.0, 0.0);
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        const Eigen::Vector2d expected_result = Eigen::Vector2d(0.123, -0.234);
        problem.evaluate_on(time, variable,
            num_collect::ode::evaluation_type{.jacobian = true});
        const Eigen::Vector2d rhs = expected_result -
            step_size * inverted_jacobian_coeff * problem.jacobian() *
                expected_result;

        Eigen::Vector2d result;
        solver.solve(rhs, result);

        comparison_approvals::verify_with_reference(result, expected_result);
    }

    SECTION("time derivative") {
        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 0.0;
        const Eigen::Vector2d variable = Eigen::Vector2d(1.0, 0.0);
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        Eigen::Vector2d target = Eigen::Vector2d::Zero();
        constexpr double coeff = 1.0;
        solver.add_time_derivative_term(step_size, coeff, target);
        CHECK(target(0) == 0.0);
        CHECK(target(1) == 0.0);
    }

    SECTION("time derivative for non-autonomous system") {
        using problem_type =
            num_prob_collect::ode::external_force_vibration_problem;
        using solver_type =
            num_collect::ode::rosenbrock::bicgstab_rosenbrock_equation_solver<
                problem_type>;

        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};

        problem_type problem;
        constexpr double time = 1.0;
        const Eigen::Vector2d variable = Eigen::Vector2d(1.0, 0.0);
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        Eigen::Vector2d target = Eigen::Vector2d::Zero();
        constexpr double coeff = 0.2;
        solver.add_time_derivative_term(step_size, coeff, target);
        CHECK_THAT(target(0),
            Catch::Matchers::WithinRel(step_size * coeff * std::cos(time)));
        CHECK(target(1) == 0.0);
    }

    SECTION("use mass if exists") {
        using problem_type = num_prob_collect::ode::implicit_kaps_problem;
        using solver_type =
            num_collect::ode::rosenbrock::bicgstab_rosenbrock_equation_solver<
                problem_type>;

        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};
        solver.tolerances(
            num_collect::ode::error_tolerances<Eigen::Vector2d>());

        constexpr double epsilon = 0.1;
        problem_type problem{epsilon};
        constexpr double time = 0.0;
        const Eigen::Vector2d variable = Eigen::Vector2d(1.0, 1.0);
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        const Eigen::Vector2d expected_result = Eigen::Vector2d(0.123, -0.234);
        problem.evaluate_on(time, variable,
            num_collect::ode::evaluation_type{.jacobian = true, .mass = true});
        const Eigen::Vector2d rhs = problem.mass() * expected_result -
            step_size * inverted_jacobian_coeff * problem.jacobian() *
                expected_result;
        Eigen::Vector2d result;
        solver.solve(rhs, result);

        comparison_approvals::verify_with_reference(result, expected_result);
    }

    SECTION("use sparse Jacobian") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_ode_problem<
                vector_type, matrix_type>;
        using solver_type =
            num_collect::ode::rosenbrock::bicgstab_rosenbrock_equation_solver<
                problem_type>;

        static constexpr double diffusion_coeff = 1.0;
        const auto test_function = [](double time, double x) {
            return std::exp(-diffusion_coeff * num_collect::pi<double> *
                       num_collect::pi<double> * time) *
                std::sin(num_collect::pi<double> * x);
        };

        constexpr num_collect::index_type num_points = 9;
        constexpr double space_resolution = 1.0 / (num_points + 1);
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        constexpr double coeff =
            diffusion_coeff / (space_resolution * space_resolution);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), -2.0 * coeff);
            if (i > 0) {
                triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i - 1), coeff);
            }
            if (i < num_points - 1) {
                triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i + 1), coeff);
            }
        }
        matrix_type stiffness_matrix(num_points, num_points);
        stiffness_matrix.setFromTriplets(triplets.begin(), triplets.end());

        const vector_type load_vector = vector_type::Zero(num_points);
        problem_type problem{stiffness_matrix, load_vector};

        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};
        solver.tolerances(num_collect::ode::error_tolerances<vector_type>());

        constexpr double time = 0.0;
        vector_type variable(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            variable(i) = test_function(
                time, static_cast<double>(i + 1) * space_resolution);
        }
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        vector_type expected_result(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            expected_result(i) = test_function(time + step_size,
                static_cast<double>(i + 1) * space_resolution);
        }
        problem.evaluate_on(time, variable,
            num_collect::ode::evaluation_type{.jacobian = true});
        const vector_type rhs = expected_result -
            step_size * inverted_jacobian_coeff * problem.jacobian() *
                expected_result;

        vector_type result(num_points);
        solver.solve(rhs, result);

        comparison_approvals::verify_with_reference(result, expected_result);
    }

    SECTION("use sparse Jacobian and mass") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_dae_problem<
                vector_type, matrix_type>;
        using solver_type =
            num_collect::ode::rosenbrock::bicgstab_rosenbrock_equation_solver<
                problem_type>;

        static constexpr double diffusion_coeff = 1.0;
        const auto test_function = [](double time, double x) {
            return std::exp(-diffusion_coeff * num_collect::pi<double> *
                       num_collect::pi<double> * time) *
                std::sin(num_collect::pi<double> * x);
        };

        constexpr double mass = 1.5;
        constexpr num_collect::index_type num_points = 9;
        constexpr double space_resolution = 1.0 / (num_points + 1);
        num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
        num_collect::util::vector<Eigen::Triplet<double>> mass_triplets;
        constexpr double coeff =
            mass * diffusion_coeff / (space_resolution * space_resolution);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            stiffness_triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), -2.0 * coeff);
            if (i > 0) {
                stiffness_triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i - 1), coeff);
            }
            if (i < num_points - 1) {
                stiffness_triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i + 1), coeff);
            }
            mass_triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), mass);
        }
        matrix_type stiffness_matrix(num_points, num_points);
        stiffness_matrix.setFromTriplets(
            stiffness_triplets.begin(), stiffness_triplets.end());
        matrix_type mass_matrix(num_points, num_points);
        mass_matrix.setFromTriplets(mass_triplets.begin(), mass_triplets.end());

        const vector_type load_vector = vector_type::Zero(num_points);
        problem_type problem{mass_matrix, stiffness_matrix, load_vector};

        constexpr double inverted_jacobian_coeff = 0.2;
        solver_type solver{inverted_jacobian_coeff};
        solver.tolerances(num_collect::ode::error_tolerances<vector_type>());

        constexpr double time = 0.0;
        vector_type variable(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            variable(i) = test_function(
                time, static_cast<double>(i + 1) * space_resolution);
        }
        constexpr double step_size = 0.01;
        solver.evaluate_and_update_jacobian(problem, time, step_size, variable);

        vector_type expected_result(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            expected_result(i) = test_function(time + step_size,
                static_cast<double>(i + 1) * space_resolution);
        }
        problem.evaluate_on(time, variable,
            num_collect::ode::evaluation_type{.jacobian = true});
        const vector_type rhs = problem.mass() * expected_result -
            step_size * inverted_jacobian_coeff * problem.jacobian() *
                expected_result;

        vector_type result(num_points);
        solver.solve(rhs, result);

        comparison_approvals::verify_with_reference(result, expected_result);
    }

    // TODO Test without Jacobian matrix.
}
