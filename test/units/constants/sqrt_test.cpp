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
 * \brief Test of sqrt function.
 */
#include "num_collect/constants/sqrt.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::sqrt", "", float, double) {
    SECTION("sqrt of negative number") {
        constexpr auto x = static_cast<TestType>(-1.0);
        constexpr TestType val = num_collect::constants::sqrt(x);
        REQUIRE(std::isnan(val));
    }

    SECTION("sqrt of zero") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr TestType val = num_collect::constants::sqrt(x);
        REQUIRE(val == num_collect::constants::zero<TestType>);
    }

    SECTION("sqrt of positive number") {
        constexpr auto true_val = static_cast<TestType>(1.234);
        constexpr TestType x = true_val * true_val;
        constexpr TestType val = num_collect::constants::sqrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("sqrt of large positive number") {
        constexpr auto true_val = static_cast<TestType>(1.234e+10);
        constexpr TestType x = true_val * true_val;
        constexpr TestType val = num_collect::constants::sqrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("sqrt of infinity") {
        constexpr TestType x = std::numeric_limits<TestType>::infinity();
        constexpr TestType val = num_collect::constants::sqrt(x);
        REQUIRE(std::isinf(val));
        REQUIRE(val > num_collect::constants::zero<TestType>);
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::sqrt (integers)", "", int, long long) {
    SECTION("sqrt of negative number") {
        constexpr auto x = static_cast<TestType>(-2);
        constexpr double val = num_collect::constants::sqrt(x);
        REQUIRE(std::isnan(val));
    }

    SECTION("sqrt of zero") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr double val = num_collect::constants::sqrt(x);
        REQUIRE(val == num_collect::constants::zero<TestType>);
    }

    SECTION("sqrt of positive number") {
        constexpr auto x = static_cast<TestType>(2);
        constexpr double val = num_collect::constants::sqrt(x);
        const double true_val = std::sqrt(static_cast<double>(x));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("sqrt of large positive number") {
        constexpr auto x = static_cast<TestType>(123456789);
        constexpr double val = num_collect::constants::sqrt(x);
        const double true_val = std::sqrt(static_cast<double>(x));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }
}
