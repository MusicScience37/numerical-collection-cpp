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
 * \brief Test of log function.
 */
#include "num_collect/constants/log.h"

#include <cmath>
#include <ostream>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::impl::log_at_compile_time", "", float, double) {
    SECTION("x < 0") {
        constexpr auto x = static_cast<TestType>(-2);
        constexpr TestType value_at_compile_time =
            num_collect::constants::impl::log_at_compile_time(x);
        const TestType value_runtime =
            num_collect::constants::impl::log_at_compile_time(x);
        CHECK(std::isnan(value_at_compile_time));
        CHECK(std::isnan(value_runtime));
    }

    SECTION("x == 0") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr TestType value_at_compile_time =
            num_collect::constants::impl::log_at_compile_time(x);
        const TestType value_runtime =
            num_collect::constants::impl::log_at_compile_time(x);
        CHECK(std::isinf(value_at_compile_time));
        CHECK(value_at_compile_time < num_collect::constants::zero<TestType>);
        CHECK(std::isinf(value_runtime));
        CHECK(value_runtime < num_collect::constants::zero<TestType>);
    }

    SECTION("x = 1e-10") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1e-10)),
            num_collect::constants::impl::log_at_compile_time, std::log);
    }

    SECTION("x = 0.9") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.9)),
            num_collect::constants::impl::log_at_compile_time, std::log);
    }

    SECTION("x = 1") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(1)),
            num_collect::constants::impl::log_at_compile_time, std::log);
    }

    SECTION("x = 3") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(3)),
            num_collect::constants::impl::log_at_compile_time, std::log);
    }

    SECTION("x = 1e+10") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1e+10)),
            num_collect::constants::impl::log_at_compile_time, std::log);
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::log", "", float, double) {
    SECTION("x < 0") {
        constexpr auto x = static_cast<TestType>(-2);
        constexpr TestType value_at_compile_time =
            num_collect::constants::log(x);
        const TestType value_runtime = num_collect::constants::log(x);
        CHECK(std::isnan(value_at_compile_time));
        CHECK(std::isnan(value_runtime));
    }

    SECTION("x == 0") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr TestType value_at_compile_time =
            num_collect::constants::log(x);
        const TestType value_runtime = num_collect::constants::log(x);
        CHECK(std::isinf(value_at_compile_time));
        CHECK(value_at_compile_time < num_collect::constants::zero<TestType>);
        CHECK(std::isinf(value_runtime));
        CHECK(value_runtime < num_collect::constants::zero<TestType>);
    }

    SECTION("x = 1e-10") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1e-10)),
            num_collect::constants::log, std::log);
    }

    SECTION("x = 0.9") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.9)),
            num_collect::constants::log, std::log);
    }

    SECTION("x = 1") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE(
            (static_cast<TestType>(1)), num_collect::constants::log, std::log);
    }

    SECTION("x = 3") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(3)), num_collect::constants::log, std::log);
    }

    SECTION("x = 1e+10") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1e+10)),
            num_collect::constants::log, std::log);
    }
}
