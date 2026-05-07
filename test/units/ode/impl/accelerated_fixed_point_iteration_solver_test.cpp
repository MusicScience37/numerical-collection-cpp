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
 * \brief Test of accelerated_fixed_point_iteration_solver class.
 */
#include "num_collect/ode/impl/accelerated_fixed_point_iteration_solver.h"

#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/constants.h"
#include "num_collect/logging/log_tag_config_node.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/ode_errors.h"

TEST_CASE("num_collect::ode::impl::accelerated_fixed_point_iteration_solver") {
    using num_collect::ode::error_tolerances;
    using num_collect::ode::impl::accelerated_fixed_point_iteration_solver;

    SECTION("solve an equation") {
        using variable_type = Eigen::Vector2d;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        const auto function = [](const variable_type& input,
                                  variable_type& output) {
            // First component: x = x + sin(x)
            output[0] = input[0] + std::sin(input[0]);
            // Second component: x = x + (1 - x^2) * 4 (Relaxation is needed.)
            output[1] = input[1] + (1.0 - input[1] * input[1]) * 4.0;
        };
        variable_type solution = variable_type(3.0, 0.7);
        const variable_type tolerance_reference = variable_type::Constant(1.0);

        constexpr double tolerance = 1e-6;
        solver.tolerances(error_tolerances<variable_type>()
                .tol_abs_error(tolerance)
                .tol_rel_error(tolerance));

        solver.solve(function, solution, tolerance_reference);

        CHECK_THAT(solution[0],
            Catch::Matchers::WithinRel(num_collect::pi<double>, tolerance));
        CHECK_THAT(solution[1], Catch::Matchers::WithinRel(1.0, tolerance));
    }

    SECTION("start iteration from the solution") {
        using variable_type = Eigen::Vector2d;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        const auto function = [](const variable_type& input,
                                  variable_type& output) {
            // First component: x = x + sin(x)
            output[0] = input[0] + std::sin(input[0]);
            // Second component: x = x + (1 - x^2) * 4 (Relaxation is needed.)
            output[1] = input[1] + (1.0 - input[1] * input[1]) * 4.0;
        };
        variable_type solution = variable_type(num_collect::pi<double>, 1.0);
        const variable_type tolerance_reference = variable_type::Constant(1.0);

        constexpr double tolerance = 1e-6;
        solver.tolerances(error_tolerances<variable_type>()
                .tol_abs_error(tolerance)
                .tol_rel_error(tolerance));

        solver.solve(function, solution, tolerance_reference);

        CHECK_THAT(solution[0],
            Catch::Matchers::WithinRel(num_collect::pi<double>, tolerance));
        CHECK_THAT(solution[1], Catch::Matchers::WithinRel(1.0, tolerance));
        CHECK(solver.iterations() == 0);
    }

    SECTION("fail to solve an equation due to no convergence") {
        // Almost same as the ordinary case, but the maximum number of
        // iterations is set to 1 to cause no convergence.
        using variable_type = Eigen::Vector2d;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        const auto function = [](const variable_type& input,
                                  variable_type& output) {
            // First component: x = x + sin(x)
            output[0] = input[0] + std::sin(input[0]);
            // Second component: x = x + (1 - x^2) * 4 (Relaxation is needed.)
            output[1] = input[1] + (1.0 - input[1] * input[1]) * 4.0;
        };
        variable_type solution = variable_type(3.0, 0.7);
        const variable_type tolerance_reference = variable_type::Constant(1.0);

        constexpr double tolerance = 1e-6;
        solver.tolerances(error_tolerances<variable_type>()
                .tol_abs_error(tolerance)
                .tol_rel_error(tolerance));

        solver.max_iterations(1);

        CHECK_THROWS_AS(solver.solve(function, solution, tolerance_reference),
            num_collect::ode::no_convergence);
    }

    SECTION(
        "fail to solve an equation due to an equation which has no solution") {
        using variable_type = Eigen::Vector2d;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        const auto function = [](const variable_type& input,
                                  variable_type& output) {
            // First component: x = x + sin(x)
            output[0] = input[0] + std::sin(input[0]);
            // Second component: x = x + 1 (No solution.)
            output[1] = input[1] + 1.0;
        };
        variable_type solution = variable_type(3.0, 0.7);
        const variable_type tolerance_reference = variable_type::Constant(1.0);

        CHECK_THROWS_AS(solver.solve(function, solution, tolerance_reference),
            num_collect::ode::linear_solver_failure);
    }

    SECTION("set the tolerance rate") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.tolerance_rate(0.5));
        CHECK_THROWS(solver.tolerance_rate(0.0));
    }

    SECTION("set the maximum size of history") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.max_history_size(1));
        CHECK_THROWS(solver.max_history_size(0));
    }

    SECTION("set the max iterations") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.max_iterations(100));
        CHECK_THROWS(solver.max_iterations(0));
    }

    SECTION("set the initial relaxation coefficient") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.initial_relaxation_coefficient(0.5));
        CHECK_THROWS(solver.initial_relaxation_coefficient(0.0));
    }

    SECTION("set the max relaxation coefficient") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.max_relaxation_coefficient(1.0));
        CHECK_THROWS(solver.max_relaxation_coefficient(0.0));
    }

    SECTION("set the min relaxation coefficient") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.min_relaxation_coefficient(1e-8));
        CHECK_THROWS(solver.min_relaxation_coefficient(0.0));
    }

    SECTION("set the relaxation coefficient reduction rate") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.relaxation_coefficient_reduction_rate(0.5));
        CHECK_THROWS(solver.relaxation_coefficient_reduction_rate(0.0));
        CHECK_THROWS(solver.relaxation_coefficient_reduction_rate(1.0));
    }

    SECTION("set the relaxation coefficient increase rate") {
        using variable_type = Eigen::VectorXd;
        using solver_type =
            accelerated_fixed_point_iteration_solver<variable_type>;
        solver_type solver;

        CHECK_NOTHROW(solver.relaxation_coefficient_increase_rate(1.05));
        CHECK_THROWS(solver.relaxation_coefficient_increase_rate(1.0));
    }
}
