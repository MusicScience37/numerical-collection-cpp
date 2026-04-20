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
 * \brief Test of inexact_newton_decomposed_full_slope_equation_solver
 * class.
 */
#include "num_collect/ode/runge_kutta/inexact_newton_decomposed_full_slope_equation_solver.h"

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
    "num_collect::ode::runge_kutta::inexact_newton_decomposed_full_slope_"
    "equation_solver") {
    using num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_equation_solver_data;
    using num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_slope_equation_solver;
    using num_collect::ode::runge_kutta::radau2a5_formula;

    SECTION("solve a scalar problem without mass") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;
        using solver_type =
            inexact_newton_decomposed_full_slope_equation_solver<problem_type,
                formula_type::stages>;
        using solver_data_type =
            inexact_newton_decomposed_full_equation_solver_data<
                solver_type::scalar_type, formula_type::stages>;
        using slope_type = solver_type::slope_vector_type;

        const solver_data_type data = solver_data_type::from_butcher_tableau(
            formula_type::slope_coeffs(), formula_type::time_coeffs());
        solver_type solver(data);

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double variable = 1.0;
        slope_type solution = slope_type::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        constexpr int stages = formula_type::stages;
        const Eigen::Vector<double, stages> update_coeffs =
            formula_type::update_coeffs();
        const double actual =
            variable + step_size * solution.dot(update_coeffs);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(actual, reference);
    }
}
