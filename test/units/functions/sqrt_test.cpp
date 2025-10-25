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
#include "num_collect/functions/sqrt.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

TEMPLATE_TEST_CASE(
    "num_collect::functions::impl::sqrt_at_compile_time", "", float, double) {
    SECTION("sqrt of negative number") {
        constexpr auto x = static_cast<TestType>(-1.0);
        constexpr TestType value_at_compile_time =
            num_collect::functions::impl::sqrt_at_compile_time(x);
        const TestType value_runtime =
            num_collect::functions::impl::sqrt_at_compile_time(x);
        CHECK(std::isnan(value_at_compile_time));
        CHECK(std::isnan(value_runtime));
    }

    SECTION("sqrt of zero") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0)),
            num_collect::functions::impl::sqrt_at_compile_time, std::sqrt);
    }

    SECTION("sqrt of positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234)),
            num_collect::functions::impl::sqrt_at_compile_time, std::sqrt);
    }

    SECTION("sqrt of large positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234e+10)),
            num_collect::functions::impl::sqrt_at_compile_time, std::sqrt);
    }

    SECTION("sqrt of infinity") {
        constexpr TestType x = std::numeric_limits<TestType>::infinity();
        constexpr TestType value_at_compile_time =
            num_collect::functions::impl::sqrt_at_compile_time(x);
        const TestType value_runtime =
            num_collect::functions::impl::sqrt_at_compile_time(x);
        CHECK(std::isinf(value_at_compile_time));
        CHECK(value_at_compile_time > static_cast<TestType>(0));
        CHECK(std::isinf(value_runtime));
        CHECK(value_runtime > static_cast<TestType>(0));
    }
}

TEMPLATE_TEST_CASE("num_collect::functions::sqrt", "", float, double) {
    SECTION("sqrt of negative number") {
        constexpr auto x = static_cast<TestType>(-1.0);
        constexpr TestType value_at_compile_time =
            num_collect::functions::sqrt(x);
        const TestType value_runtime = num_collect::functions::sqrt(x);
        CHECK(std::isnan(value_at_compile_time));
        CHECK(std::isnan(value_runtime));
    }

    SECTION("sqrt of zero") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0)),
            num_collect::functions::sqrt, std::sqrt);
    }

    SECTION("sqrt of positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234)),
            num_collect::functions::sqrt, std::sqrt);
    }

    SECTION("sqrt of large positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234e+10)),
            num_collect::functions::sqrt, std::sqrt);
    }

    SECTION("sqrt of infinity") {
        constexpr TestType x = std::numeric_limits<TestType>::infinity();
        constexpr TestType value_at_compile_time =
            num_collect::functions::sqrt(x);
        const TestType value_runtime = num_collect::functions::sqrt(x);
        CHECK(std::isinf(value_at_compile_time));
        CHECK(value_at_compile_time > static_cast<TestType>(0));
        CHECK(std::isinf(value_runtime));
        CHECK(value_runtime > static_cast<TestType>(0));
    }
}

TEMPLATE_TEST_CASE(
    "num_collect::functions::sqrt (integers)", "", int, long long) {
    SECTION("sqrt of negative number") {
        constexpr auto x = static_cast<TestType>(-2);
        constexpr double value_at_compile_time =
            num_collect::functions::sqrt(x);
        const double value_runtime = num_collect::functions::sqrt(x);
        CHECK(std::isnan(value_at_compile_time));
        CHECK(std::isnan(value_runtime));
    }

    SECTION("sqrt of zero") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0)),
            num_collect::functions::sqrt, std::sqrt);
    }

    SECTION("sqrt of positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(2)),
            num_collect::functions::sqrt, std::sqrt);
    }

    SECTION("sqrt of large positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(123456789)),
            num_collect::functions::sqrt, std::sqrt);
    }
}
