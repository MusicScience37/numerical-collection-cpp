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
 * \brief Test of inexact_newton_decomposed_full_equation_solver_data
 * class.
 */
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "eigen_approx.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE(
    "num_collect::ode::runge_kutta::inexact_newton_decomposed_full_equation_"
    "solver_data") {
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
