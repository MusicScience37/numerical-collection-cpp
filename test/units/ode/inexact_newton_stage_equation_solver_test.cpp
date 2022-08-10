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
 * \brief Test of inexact_newton_stage_equation_solver class.
 */
#include "num_collect/ode/inexact_newton_stage_equation_solver.h"

#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::inexact_newton_stage_equation_solver(scalar)") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type =
        num_collect::ode::inexact_newton_stage_equation_solver<problem_type>;

    SECTION("solve for implicit Euler method") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-4;
        constexpr double init_var = 1.0;
        constexpr double solution_coeff = 1.0;

        solver.update_jacobian(
            problem, init_time, step_size, init_var, solution_coeff);
        double solution{0.0};
        solver.init(init_time, step_size, init_var, solution);
        solver.solve();

        const double variable = init_var + step_size * solution;
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(variable, reference);
    }
}

TEST_CASE("num_collect::ode::inexact_newton_stage_equation_solver(vector)") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::inexact_newton_stage_equation_solver<problem_type>;

    SECTION("solve for implicit Euler method") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-4;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        constexpr double solution_coeff = 1.0;

        solver.update_jacobian(
            problem, init_time, step_size, init_var, solution_coeff);
        Eigen::Vector2d solution;
        solver.init(init_time, step_size, init_var, solution);
        solver.solve();

        const Eigen::Vector2d variable = init_var + step_size * solution;
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(variable, reference);
    }
}
