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
 * \brief Test of sdirk6_formula class.
 */
#include "num_collect/ode/runge_kutta/sdirk6_formula.h"

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::sdirk6_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::sdirk6_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 9);
        STATIC_REQUIRE(formula_type::order == 6);
        STATIC_REQUIRE(formula_type::lesser_order == 5);

        CHECK_THAT(formula_type::c2 + formula_type::c3 + formula_type::c4 +
                formula_type::c5 + formula_type::c6 + formula_type::c7 +
                formula_type::c8 + formula_type::c9,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::cw2 + formula_type::cw3 + formula_type::cw4 +
                formula_type::cw5 + formula_type::cw6 + formula_type::cw7 +
                formula_type::cw8 + formula_type::cw9,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::ce2 + formula_type::ce3 + formula_type::ce4 +
                formula_type::ce5 + formula_type::ce6 + formula_type::ce7 +
                formula_type::ce8 + formula_type::ce9,
            Catch::Matchers::WithinAbs(0.0, 1e-10));
    }

    SECTION("initialize") {
        auto formula = formula_type(problem_type());
        (void)formula;
    }

    SECTION("step") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-4;
        constexpr double prev_var = 1.0;
        double next_var = 0.0;
        formula.step(time, step_size, prev_var, next_var);

        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(next_var, reference);
    }

    SECTION("step_embedded") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double prev_var = 1.0;
        double next_var = 0.0;
        double error = 0.0;
        formula.step_embedded(time, step_size, prev_var, next_var, error);

        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference_and_error(
            next_var, error, reference);
    }
}
