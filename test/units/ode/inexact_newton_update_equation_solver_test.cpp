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
 * \brief Test of inexact_newton_update_equation_solver class.
 */
#include "num_collect/ode/inexact_newton_update_equation_solver.h"

#include <initializer_list>
#include <limits>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "num_collect/ode/concepts/update_equation_solver.h"  // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::inexact_newton_update_equation_solver(scalar)") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type =
        num_collect::ode::inexact_newton_update_equation_solver<problem_type>;

    SECTION("check concept") {
        STATIC_REQUIRE(
            num_collect::ode::concepts::update_equation_solver<solver_type>);
    }

    SECTION("solve for Hammer-Hollingsworth method") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const double init_var = 1.0;

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const double k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        double z2{0.0};
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const double k2 = (z2 - solver.solution_offset()) / (step_size * a22);

        const double variable = init_var + step_size * (c1 * k1 + c2 * k2);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("failure in solving an equation") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1.0;
        constexpr double init_var = 1.0;
        constexpr double slope_coeff = 1.0;

        CHECK_THROWS(solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff));
    }

    SECTION("call solve function before initialization") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        constexpr double init_var = 1.0;
        constexpr double slope_coeff = 1.0;
        const double solution_offset = 1.0;

        CHECK_THROWS(solver.solve());

        solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff);
        CHECK_THROWS(solver.solve());

        double solution{0.0};
        solver.init(solution_offset, solution);
        CHECK_NOTHROW(solver.solve());
    }
}

TEST_CASE("num_collect::ode::inexact_newton_update_equation_solver(vector)") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::inexact_newton_update_equation_solver<problem_type>;

    SECTION("check concept") {
        STATIC_REQUIRE(
            num_collect::ode::concepts::update_equation_solver<solver_type>);
    }

    SECTION("solve for Hammer-Hollingsworth method") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        Eigen::Vector2d z2 = Eigen::Vector2d::Zero();
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k2 =
            (z2 - solver.solution_offset()) / (step_size * a22);

        const Eigen::Vector2d variable =
            init_var + step_size * (c1 * k1 + c2 * k2);
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("failure in solving an equation") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const Eigen::Vector2d init_var{
            {1.0, std::numeric_limits<double>::quiet_NaN()}};
        constexpr double slope_coeff = 1.0;
        const Eigen::Vector2d solution_offset{{0.0, 0.0}};

        solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff);
        Eigen::Vector2d solution = Eigen::Vector2d::Zero();
        solver.init(solution_offset, solution);
        CHECK_THROWS(solver.solve());
    }

    SECTION("call solve function before initialization") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const Eigen::Vector2d init_var{{1.0, 0.0}};
        constexpr double slope_coeff = 1.0;
        const Eigen::Vector2d solution_offset{{0.0, 0.0}};

        CHECK_THROWS(solver.solve());

        solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff);
        CHECK_THROWS(solver.solve());

        Eigen::Vector2d solution = Eigen::Vector2d::Zero();
        solver.init(solution_offset, solution);
        CHECK_NOTHROW(solver.solve());
    }
}
