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
 * \brief Test of cbrt function.
 */
#include "num_collect/constants/cbrt.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::cbrt", "", float, double) {
    SECTION("cbrt of negative number") {
        constexpr auto x = static_cast<TestType>(-1.234);
        constexpr auto val = num_collect::constants::cbrt(x);
        const auto true_val = std::cbrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("cbrt of zero") {
        constexpr auto x = static_cast<TestType>(0.0);
        constexpr auto val = num_collect::constants::cbrt(x);
        const auto true_val = std::cbrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("cbrt of positive number") {
        constexpr auto x = static_cast<TestType>(1.234);
        constexpr auto val = num_collect::constants::cbrt(x);
        const auto true_val = std::cbrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("cbrt of large positive number") {
        constexpr auto x = static_cast<TestType>(1.234e+10);
        constexpr auto val = num_collect::constants::cbrt(x);
        const auto true_val = std::cbrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("cbrt of infinity") {
        constexpr TestType x = std::numeric_limits<TestType>::infinity();
        constexpr TestType val = num_collect::constants::cbrt(x);
        REQUIRE(std::isinf(val));
        REQUIRE(val > num_collect::constants::zero<TestType>);
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::cbrt (integers)", "", int, long long) {
    SECTION("cbrt of negative number") {
        constexpr auto x = static_cast<TestType>(-5);
        constexpr auto val = num_collect::constants::cbrt(x);
        const double true_val = std::cbrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("cbrt of zero") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr auto val = num_collect::constants::cbrt(x);
        const double true_val = std::cbrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("cbrt of positive number") {
        constexpr auto x = static_cast<TestType>(10);
        constexpr auto val = num_collect::constants::cbrt(x);
        const double true_val = std::cbrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }
}
