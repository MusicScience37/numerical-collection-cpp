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
#include "num_collect/functions/log1p.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

TEMPLATE_TEST_CASE(
    "num_collect::functions::impl::log1p_at_compile_time", "", float, double) {
    SECTION("x < -1") {
        constexpr auto x = static_cast<TestType>(-2);
        constexpr TestType value_at_compile_time =
            num_collect::functions::impl::log1p_at_compile_time(x);
        const TestType value_runtime =
            num_collect::functions::impl::log1p_at_compile_time(x);
        CHECK(std::isnan(value_at_compile_time));
        CHECK(std::isnan(value_runtime));
    }

    SECTION("x == -1") {
        constexpr auto x = static_cast<TestType>(-1);
        constexpr TestType value_at_compile_time =
            num_collect::functions::impl::log1p_at_compile_time(x);
        const TestType value_runtime =
            num_collect::functions::impl::log1p_at_compile_time(x);
        CHECK(std::isinf(value_at_compile_time));
        CHECK(value_at_compile_time < static_cast<TestType>(0));
        CHECK(std::isinf(value_runtime));
        CHECK(value_runtime < static_cast<TestType>(0));
    }

    SECTION("x = -0.9999") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.9999)),
            num_collect::functions::impl::log1p_at_compile_time, std::log1p);
    }

    SECTION("x = -0.1") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.1)),
            num_collect::functions::impl::log1p_at_compile_time, std::log1p);
    }

    SECTION("x = -0.00001") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.00001)),
            num_collect::functions::impl::log1p_at_compile_time, std::log1p);
    }

    SECTION("x = 0") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0)),
            num_collect::functions::impl::log1p_at_compile_time, std::log1p);
    }

    SECTION("x = 0.00001") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.00001)),
            num_collect::functions::impl::log1p_at_compile_time, std::log1p);
    }

    SECTION("x = 3") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(3)),
            num_collect::functions::impl::log1p_at_compile_time, std::log1p);
    }

    SECTION("x = 1e+10") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1e+10)),
            num_collect::functions::impl::log1p_at_compile_time, std::log1p);
    }
}

TEMPLATE_TEST_CASE("num_collect::functions::log1p", "", float, double) {
    SECTION("x < -1") {
        constexpr auto x = static_cast<TestType>(-2);
        constexpr TestType value_at_compile_time =
            num_collect::functions::log1p(x);
        const TestType value_runtime = num_collect::functions::log1p(x);
        CHECK(std::isnan(value_at_compile_time));
        CHECK(std::isnan(value_runtime));
    }

    SECTION("x == -1") {
        constexpr auto x = static_cast<TestType>(-1);
        constexpr TestType value_at_compile_time =
            num_collect::functions::log1p(x);
        const TestType value_runtime = num_collect::functions::log1p(x);
        CHECK(std::isinf(value_at_compile_time));
        CHECK(value_at_compile_time < static_cast<TestType>(0));
        CHECK(std::isinf(value_runtime));
        CHECK(value_runtime < static_cast<TestType>(0));
    }

    SECTION("x = -0.9999") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.9999)),
            num_collect::functions::log1p, std::log1p);
    }

    SECTION("x = -0.1") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.1)),
            num_collect::functions::log1p, std::log1p);
    }

    SECTION("x = -0.00001") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.00001)),
            num_collect::functions::log1p, std::log1p);
    }

    SECTION("x = 0") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0)),
            num_collect::functions::log1p, std::log1p);
    }

    SECTION("x = 0.00001") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.00001)),
            num_collect::functions::log1p, std::log1p);
    }

    SECTION("x = 3") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(3)),
            num_collect::functions::log1p, std::log1p);
    }

    SECTION("x = 1e+10") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1e+10)),
            num_collect::functions::log1p, std::log1p);
    }
}
