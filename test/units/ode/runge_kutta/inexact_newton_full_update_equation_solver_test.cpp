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
 * \brief Test of inexact_newton_full_update_equation_solver class.
 */
#include "num_collect/ode/runge_kutta/inexact_newton_full_update_equation_solver.h"

#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/constants.h"
#include "num_collect/ode/concepts/update_equation_solver.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/util/vector.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_exponential_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE(
    "num_collect::ode::runge_kutta::inexact_newton_full_update_equation_"
    "solver") {
    using num_collect::ode::runge_kutta::
        inexact_newton_full_update_equation_solver;

    // Radau IIA method of order 3.
    // This formula is stiffly accurate, so the update in the second stage can
    // be used as the update of the variable.
    const Eigen::Matrix2d slope_coeffs{{5.0 / 12.0, -1.0 / 12.0},
        {
            3.0 / 4.0,
            1.0 / 4.0,
        }};
    const Eigen::Vector2d time_coeffs{{1.0 / 3.0, 1.0}};
    const Eigen::Vector2d update_coeffs{{3.0 / 4.0, 1.0 / 4.0}};
    constexpr int num_stages = 2;

    SECTION("solve a scalar problem without mass") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using solver_type =
            inexact_newton_full_update_equation_solver<problem_type,
                num_stages>;

        solver_type solver(slope_coeffs, time_coeffs, update_coeffs);

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double step_size = 1e-3;
        constexpr double variable = 1.0;
        Eigen::Vector2d solution = Eigen::Vector2d::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const double actual = variable + solution(1);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(actual, reference);
    }

    SECTION("solve a scalar problem with mass") {
        using problem_type =
            num_prob_collect::ode::implicit_exponential_problem;
        using solver_type =
            inexact_newton_full_update_equation_solver<problem_type,
                num_stages>;

        solver_type solver(slope_coeffs, time_coeffs, update_coeffs);

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double step_size = 1e-3;
        constexpr double variable = 1.0;
        Eigen::Vector2d solution = Eigen::Vector2d::Zero();
        solver.init(problem, time, step_size, variable, solution);
        REQUIRE_NOTHROW(solver.solve());

        const double actual = variable + solution(1);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(actual, reference);
    }
}
