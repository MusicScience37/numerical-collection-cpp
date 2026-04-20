/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of inexact_newton_decomposed_full_update_equation_solver
 * class.
 */
#include "num_collect/ode/runge_kutta/inexact_newton_decomposed_full_update_equation_solver.h"

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "comparison_approvals.h"
#include "eigen_approx.h"
#include "num_collect/base/constants.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_exponential_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE(
    "num_collect::ode::runge_kutta::inexact_newton_decomposed_full_update_"
    "equation_solver_data") {
    using num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_equation_solver_data;
    using num_collect::ode::runge_kutta::radau2a5_formula;

    SECTION("create data") {
        using formula_type =
            radau2a5_formula<num_prob_collect::ode::exponential_problem>;
        const auto& slope_coeffs = formula_type::slope_coeffs();
        const auto& time_coeffs = formula_type::time_coeffs();

        using data_type =
            inexact_newton_decomposed_full_equation_solver_data<double,
                formula_type::stages>;
        data_type data =
            data_type::from_butcher_tableau(slope_coeffs, time_coeffs);

        // Inputs.
        CHECK_THAT(data.time_coeffs(), eigen_approx(time_coeffs));

        // Eigenvectors.
        const Eigen::Matrix3d eigenvectors_inverse_times_eigenvectors =
            data.eigenvectors_inverse() * data.eigenvectors();
        const Eigen::Matrix3d identity = Eigen::Matrix3d::Identity();
        CHECK_THAT(
            eigenvectors_inverse_times_eigenvectors, eigen_approx(identity));

        // Block-diagonal matrix.
        const Eigen::Matrix3d reconstructed_slope_coeffs = data.eigenvectors() *
            data.block_diagonal_matrix() * data.eigenvectors_inverse();
        CHECK_THAT(reconstructed_slope_coeffs, eigen_approx(slope_coeffs));
    }
}

TEST_CASE(
    "num_collect::ode::runge_kutta::inexact_newton_decomposed_full_update_"
    "equation_solver") {
    using num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_equation_solver_data;
    using num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_update_equation_solver;
    using num_collect::ode::runge_kutta::radau2a5_formula;

    SECTION("solve a scalar problem without mass") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_update_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;
        using update_type = solver_type::update_vector_type;

        const solver_data_type data = solver_data_type::from_butcher_tableau(
            formula_type::slope_coeffs(), formula_type::time_coeffs());
        solver_type solver(data);

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double variable = 1.0;
        update_type solution = update_type::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const double actual = variable + solution(solution.size() - 1);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(actual, reference);
    }

    SECTION("solve a scalar problem with mass") {
        using problem_type =
            num_prob_collect::ode::implicit_exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_update_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;
        using update_type = solver_type::update_vector_type;

        const solver_data_type data = solver_data_type::from_butcher_tableau(
            formula_type::slope_coeffs(), formula_type::time_coeffs());
        solver_type solver(data);

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double variable = 1.0;
        update_type solution = update_type::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const double actual = variable + solution(solution.size() - 1);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(actual, reference);
    }

    SECTION("solve a multi-variate problem without mass (dense)") {
        using problem_type = num_prob_collect::ode::spring_movement_problem;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_update_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;
        using update_type = solver_type::update_vector_type;

        const solver_data_type data = solver_data_type::from_butcher_tableau(
            formula_type::slope_coeffs(), formula_type::time_coeffs());
        solver_type solver(data);

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        const Eigen::Vector2d variable{{1.0, 0.0}};
        update_type solution = update_type::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const Eigen::Vector2d actual = variable + solution.tail<2>();
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(actual, reference);
    }

    SECTION("solve a multi-variate problem with mass (dense)") {
        using problem_type = num_prob_collect::ode::implicit_kaps_problem;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_update_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;
        using update_type = solver_type::update_vector_type;

        const solver_data_type data = solver_data_type::from_butcher_tableau(
            formula_type::slope_coeffs(), formula_type::time_coeffs());
        solver_type solver(data);

        constexpr double epsilon = 0.1;
        problem_type problem{epsilon};
        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        const Eigen::Vector2d variable{{1.0, 1.0}};
        update_type solution = update_type::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const Eigen::Vector2d actual = variable + solution.tail<2>();
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::exp(-2.0 * step_size), std::exp(-step_size));
        comparison_approvals::verify_with_reference(actual, reference);
    }

    // This name is made short due to the limitation of Windows.
    SECTION("solve a multi-variate without mass sparse") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_ode_problem<
                vector_type, matrix_type>;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_update_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;
        using update_type = solver_type::update_vector_type;

        const solver_data_type data = solver_data_type::from_butcher_tableau(
            formula_type::slope_coeffs(), formula_type::time_coeffs());
        solver_type solver(data);

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

        constexpr double time = 0.0;
        constexpr double step_size = 0.01;
        vector_type variable(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            variable(i) = test_function(
                time, static_cast<double>(i + 1) * space_resolution);
        }

        update_type solution =
            update_type::Zero(variable.size() * formula_type::stages);
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const vector_type actual = variable + solution.tail(variable.size());
        vector_type reference(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            reference(i) = test_function(time + step_size,
                static_cast<double>(i + 1) * space_resolution);
        }
        comparison_approvals::verify_with_reference(actual, reference);
    }

    SECTION("solve a multi-variate problem with mass (sparse)") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_dae_problem<
                vector_type, matrix_type>;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_update_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;
        using update_type = solver_type::update_vector_type;

        const solver_data_type data = solver_data_type::from_butcher_tableau(
            formula_type::slope_coeffs(), formula_type::time_coeffs());
        solver_type solver(data);

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

        constexpr double time = 0.0;
        constexpr double step_size = 0.01;
        vector_type variable(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            variable(i) = test_function(
                time, static_cast<double>(i + 1) * space_resolution);
        }

        update_type solution =
            update_type::Zero(variable.size() * formula_type::stages);
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const vector_type actual = variable + solution.tail(variable.size());
        vector_type reference(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            reference(i) = test_function(time + step_size,
                static_cast<double>(i + 1) * space_resolution);
        }
        comparison_approvals::verify_with_reference(actual, reference);
    }
}
