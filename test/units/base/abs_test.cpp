/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of abs function.
 */
#include "num_collect/base/abs.h"

#include <cmath>
#include <complex>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::abs") {
    SECTION("abs of float") {
        constexpr float var = -2.345F;
        const float abs_val = num_collect::abs(var);
        const float true_abs = std::abs(var);
        REQUIRE_THAT(abs_val, Catch::Matchers::WithinRel(true_abs));
    }

    SECTION("abs of double") {
        constexpr double var = -2.345;
        const double abs_val = num_collect::abs(var);
        const double true_abs = std::abs(var);
        REQUIRE_THAT(abs_val, Catch::Matchers::WithinRel(true_abs));
    }

    SECTION("abs of long double") {
        constexpr long double var = -2.345;
        const auto abs_val = static_cast<double>(num_collect::abs(var));
        const auto true_abs = static_cast<double>(std::abs(var));
        REQUIRE_THAT(abs_val, Catch::Matchers::WithinRel(true_abs));
    }

    SECTION("abs of complex") {
        const auto var = std::complex<double>(1.0, -2.0);
        const double abs_val = num_collect::abs(var);
        const double true_abs = std::abs(var);
        REQUIRE_THAT(abs_val, Catch::Matchers::WithinRel(true_abs));
    }

    SECTION("abs of int") {
        const int var = -5;
        const int abs_val = num_collect::abs(var);
        const int true_abs = std::abs(var);
        CHECK(abs_val == true_abs);
    }

    SECTION("abs of unsigned int") {
        const unsigned int var = 5U;
        const unsigned int abs_val = num_collect::abs(var);
        CHECK(abs_val == var);
    }
}
