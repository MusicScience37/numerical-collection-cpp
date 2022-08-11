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
 * \brief Test of sdirk_formula class.
 */
#include "num_collect/ode/runge_kutta/sdirk4_formula.h"

#include <cmath>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/external_force_vibration_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::sdirk4_formula") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::sdirk4_formula<problem_type>;

    SECTION("static definition") {
        STATIC_REQUIRE(formula_type::stages == 5);
        STATIC_REQUIRE(formula_type::order == 4);
        STATIC_REQUIRE(formula_type::lesser_order == 3);

        CHECK_THAT(
            formula_type::ad, Catch::Matchers::WithinRel(formula_type::b1));
        CHECK_THAT(formula_type::a21 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b2));
        CHECK_THAT(formula_type::a31 + formula_type::a32 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b3));
        CHECK_THAT(formula_type::a41 + formula_type::a42 + formula_type::a43 +
                formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b4));
        CHECK_THAT(formula_type::a51 + formula_type::a52 + formula_type::a53 +
                formula_type::a54 + formula_type::ad,
            Catch::Matchers::WithinRel(formula_type::b5));
        CHECK_THAT(formula_type::c1 + formula_type::c2 + formula_type::c3 +
                formula_type::c4 + formula_type::c5,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::cw1 + formula_type::cw2 + formula_type::cw3 +
                formula_type::cw4 + formula_type::cw5,
            Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(formula_type::ce1 + formula_type::ce2 + formula_type::ce3 +
                formula_type::ce5,
            Catch::Matchers::WithinAbs(0.0, 1e-10));  // NOLINT
    }
}
