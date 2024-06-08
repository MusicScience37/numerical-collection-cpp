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
 * \brief Test of initial_step_size_calculator class.
 */
#include "num_collect/ode/initial_step_size_calculator.h"

#include <ApprovalTests.hpp>
#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/step_size_limits.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/pendulum_movement_problem.h"

TEST_CASE("num_collect::ode::initial_step_size_calculator(actual problem)") {
    using num_collect::ode::error_tolerances;
    using num_collect::ode::initial_step_size_calculator;
    using num_collect::ode::step_size_limits;
    using num_collect::ode::rosenbrock::ros3w_formula;
    using num_collect::ode::runge_kutta::rkf45_formula;
    using num_prob_collect::ode::exponential_problem;
    using num_prob_collect::ode::pendulum_movement_problem;

    SECTION("exponential problem with RKF45 formula") {
        using problem_type = exponential_problem;
        using formula_type = rkf45_formula<problem_type>;
        using calculator_type = initial_step_size_calculator<formula_type>;

        problem_type problem;
        calculator_type calculator;
        constexpr double initial_time = 0.0;
        constexpr double initial_variable = 1.0;
        const auto limits =
            step_size_limits<double>().lower_limit(1e-8).upper_limit(1e+10);
        const auto tolerances =
            error_tolerances<double>().tol_rel_error(1e-6).tol_abs_error(1e-6);

        const double step_size = calculator.calculate(
            problem, initial_time, initial_variable, limits, tolerances);
        ApprovalTests::Approvals::verify(fmt::format("{:.3e}", step_size));
    }

    SECTION("pendulum problem with ROS3w formula") {
        using problem_type = pendulum_movement_problem;
        using formula_type = ros3w_formula<problem_type>;
        using calculator_type = initial_step_size_calculator<formula_type>;

        problem_type problem;
        calculator_type calculator;
        constexpr double initial_time = 0.0;
        const auto initial_variable = Eigen::Vector2d{{0.0, 1.0}};
        const auto limits =
            step_size_limits<double>().lower_limit(1e-8).upper_limit(1e+10);
        const auto tolerances =
            error_tolerances<Eigen::Vector2d>()
                .tol_rel_error(Eigen::Vector2d{{1e-6, 1e-6}})
                .tol_abs_error(Eigen::Vector2d{{1e-6, 1e-6}});

        const double step_size = calculator.calculate(
            problem, initial_time, initial_variable, limits, tolerances);
        ApprovalTests::Approvals::verify(fmt::format("{:.3e}", step_size));
    }
}
