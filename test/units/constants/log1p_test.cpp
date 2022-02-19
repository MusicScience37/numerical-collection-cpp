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
 * \brief Test of log1p function.
 */
#include "num_collect/constants/log1p.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::log1p", "", float, double) {
    SECTION("x < -1") {
        constexpr auto x = static_cast<TestType>(-2);
        constexpr TestType val = num_collect::constants::log1p(x);
        REQUIRE(std::isnan(val));
    }

    SECTION("x == -1") {
        constexpr auto x = static_cast<TestType>(-1);
        constexpr TestType val = num_collect::constants::log1p(x);
        REQUIRE(std::isinf(val));
        REQUIRE(val < num_collect::constants::zero<TestType>);
    }

    SECTION("x = -0.9999") {
        constexpr auto x = static_cast<TestType>(-0.9999);
        constexpr TestType val = num_collect::constants::log1p(x);
        const auto reference = std::log1p(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = -0.1") {
        constexpr auto x = static_cast<TestType>(-0.1);
        constexpr TestType val = num_collect::constants::log1p(x);
        const auto reference = std::log1p(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = -0.00001") {
        constexpr auto x = static_cast<TestType>(-0.00001);
        constexpr TestType val = num_collect::constants::log1p(x);
        const auto reference = std::log1p(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = 0") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr TestType val = num_collect::constants::log1p(x);
        REQUIRE(val == num_collect::constants::zero<TestType>);
    }

    SECTION("x = 0.00001") {
        constexpr auto x = static_cast<TestType>(0.00001);
        constexpr TestType val = num_collect::constants::log1p(x);
        const auto reference = std::log1p(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = 3") {
        constexpr auto x = static_cast<TestType>(3);
        constexpr TestType val = num_collect::constants::log1p(x);
        const auto reference = std::log1p(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = 1e+10") {
        constexpr auto x = static_cast<TestType>(1e+10);
        constexpr TestType val = num_collect::constants::log1p(x);
        const auto reference = std::log1p(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}
