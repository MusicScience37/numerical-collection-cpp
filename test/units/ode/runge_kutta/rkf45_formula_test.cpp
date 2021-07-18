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
 * \brief Test of rkf45_formula class.
 */
#include "num_collect/ode/runge_kutta/rkf45_formula.h"

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::rkf45_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::rkf45_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 6);
        STATIC_REQUIRE(formula_type::order == 5);
        STATIC_REQUIRE(formula_type::lesser_order == 4);
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
        constexpr double tol = 1e-12;
        REQUIRE_THAT(next_var, Catch::Matchers::WithinRel(reference, tol));
    }

    SECTION("step_embedded") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double prev_var = 1.0;
        double next_var = 0.0;
        double weak_next_var = 0.0;
        formula.step_embedded(
            time, step_size, prev_var, next_var, weak_next_var);

        const double reference = std::exp(step_size);
        constexpr double tol = 1e-8;
        REQUIRE_THAT(next_var, Catch::Matchers::WithinRel(reference, tol));
        REQUIRE_THAT(weak_next_var, Catch::Matchers::WithinRel(reference, tol));
        REQUIRE_THAT(
            next_var - weak_next_var, Catch::Matchers::WithinAbs(0.0, tol));
    }
}
