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
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "comparison_approvals.h"
#include "eigen_approx.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE(
    "num_collect::ode::runge_kutta::inexact_newton_decomposed_full_update_"
    "equation_solver_data") {
    using num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_update_equation_solver_data;
    using num_collect::ode::runge_kutta::radau2a5_formula;

    SECTION("create data") {
        using formula_type =
            radau2a5_formula<num_prob_collect::ode::exponential_problem>;
        const auto& slope_coeffs = formula_type::slope_coeffs();
        const auto& time_coeffs = formula_type::time_coeffs();
        const auto& update_coeffs = formula_type::update_coeffs();

        using data_type =
            inexact_newton_decomposed_full_update_equation_solver_data<double,
                formula_type::stages>;
        data_type data(slope_coeffs, time_coeffs, update_coeffs);

        // Inputs.
        CHECK_THAT(data.slope_coeffs(), eigen_approx(slope_coeffs));
        CHECK_THAT(data.time_coeffs(), eigen_approx(time_coeffs));
        CHECK_THAT(data.update_coeffs(), eigen_approx(update_coeffs));

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
        inexact_newton_decomposed_full_update_equation_solver;
    using num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_update_equation_solver_data;
    using num_collect::ode::runge_kutta::radau2a5_formula;

    SECTION("solve a scalar problem without mass") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_update_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_update_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;

        const solver_data_type data(formula_type::slope_coeffs(),
            formula_type::time_coeffs(), formula_type::update_coeffs());
        solver_type solver(data);

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double variable = 1.0;
        Eigen::Vector3d solution = Eigen::Vector3d::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const double actual = variable + solution(2);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(actual, reference);
    }
}
