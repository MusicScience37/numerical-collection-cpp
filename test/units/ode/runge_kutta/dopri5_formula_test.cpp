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
 * \brief Test of dopri5_formula class.
 */
#include "num_collect/ode/runge_kutta/dopri5_formula.h"

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::dopri5_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::dopri5_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 7);
        STATIC_REQUIRE(formula_type::order == 5);
        STATIC_REQUIRE(formula_type::lesser_order == 4);

        CHECK_THAT(
            formula_type::a21, Catch::Matchers::WithinRel(formula_type::b2));
        CHECK_THAT(formula_type::a31 + formula_type::a32,
            Catch::Matchers::WithinRel(formula_type::b3));
        CHECK_THAT(formula_type::a41 + formula_type::a42 + formula_type::a43,
            Catch::Matchers::WithinRel(formula_type::b4));
        CHECK_THAT(formula_type::a51 + formula_type::a52 + formula_type::a53 +
                formula_type::a54,
            Catch::Matchers::WithinRel(formula_type::b5));
        CHECK_THAT(formula_type::a61 + formula_type::a62 + formula_type::a63 +
                formula_type::a64 + formula_type::a65,
            Catch::Matchers::WithinRel(formula_type::b6));
        CHECK_THAT(formula_type::a71 + formula_type::a72 + formula_type::a73 +
                formula_type::a74 + formula_type::a75 + formula_type::a76,
            Catch::Matchers::WithinRel(formula_type::b7));
        CHECK_THAT(formula_type::c1 + formula_type::c3 + formula_type::c4 +
                formula_type::c5 + formula_type::c6,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::cw1 + formula_type::cw3 + formula_type::cw4 +
                formula_type::cw5 + formula_type::cw6 + formula_type::cw7,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::ce1 + formula_type::ce3 + formula_type::ce4 +
                formula_type::ce5 + formula_type::ce6 + formula_type::ce7,
            Catch::Matchers::WithinAbs(0.0, 1e-10));  // NOLINT
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
