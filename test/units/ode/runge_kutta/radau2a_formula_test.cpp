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
 * \brief Test of radau2a_formula class.
 */
#include "num_collect/ode/runge_kutta/radau2a_formula.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_collect/logging/logger.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEMPLATE_TEST_CASE_SIG("num_collect::ode::runge_kutta::radau2a_formula", "",
    ((int Stages), Stages), (4), (5), (6), (7), (8), (9)) {
    using num_collect::ode::runge_kutta::radau2a_formula;

    num_collect::logging::logger logger;

    SECTION("static definition") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a_formula<Stages, problem_type>;

        STATIC_REQUIRE(formula_type::stages == Stages);
        STATIC_REQUIRE(formula_type::order == 2 * Stages - 1);
    }

    SECTION("check coefficients") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a_formula<Stages, problem_type>;

        const auto slope_coeffs = formula_type::slope_coeffs();
        const auto time_coeffs = formula_type::time_coeffs();
        const auto update_coeffs = formula_type::update_coeffs();

        for (int i = 0; i < formula_type::stages; ++i) {
            INFO("i = " << i);
            const double slope_sum = slope_coeffs.row(i).sum();
            const double time_coeff = time_coeffs(i);
            NUM_COLLECT_LOG_DEBUG(logger,
                "i= {}, slope_sum = {}, time_coeff = {}, error = {}", i,
                slope_sum, time_coeff, slope_sum - time_coeff);
            CHECK_THAT(slope_sum, Catch::Matchers::WithinRel(time_coeff));
        }
        const double update_sum = update_coeffs.sum();
        NUM_COLLECT_LOG_DEBUG(logger, "update_sum = {}, error = {}", update_sum,
            update_sum - 1.0);
        CHECK_THAT(update_sum, Catch::Matchers::WithinRel(1.0));
    }

    SECTION("step in one-dimensional problem") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a_formula<Stages, problem_type>;
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double prev_var = 1.0;
        double next_var = 0.0;
        formula.step(time, step_size, prev_var, next_var);

        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(next_var, reference);
    }

    SECTION("step in multi-dimensional problem") {
        using problem_type = num_prob_collect::ode::spring_movement_problem;
        using formula_type = radau2a_formula<Stages, problem_type>;
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        const Eigen::Vector2d prev_var(1.0, 0.0);
        Eigen::Vector2d next_var;
        formula.step(time, step_size, prev_var, next_var);

        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(next_var, reference);
    }
}
