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
 * \brief Test of semi_implicit_formula_solver class.
 */
#include "num_collect/ode/runge_kutta/semi_implicit_formula_solver.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "eigen_approx.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE(
    "num_collect::ode::runge_kutta::semi_implicit_formula_solver<spring_"
    "movement_problem, modified_newton_raphson_tag>") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using strategy_type = num_collect::ode::runge_kutta::
        implicit_formula_solver_strategies::modified_newton_raphson_tag;
    using solver_type =
        num_collect::ode::runge_kutta::semi_implicit_formula_solver<
            problem_type, strategy_type>;

    SECTION("solve for implicit Euler method") {
        auto solver = solver_type(problem_type());

        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-4;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        constexpr double k_coeff = 1.0;
        REQUIRE_NOTHROW(solver.solve(init_time, step_size, init_var, k_coeff));

        const Eigen::Vector2d variable = init_var + step_size * solver.k();
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        constexpr double tol = 1e-8;
        REQUIRE_THAT(variable, eigen_approx(reference, tol));
    }
}
