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
#include <limits>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

TEMPLATE_TEST_CASE("num_collect::constants::cbrt", "", float, double) {
    SECTION("cbrt of negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-1.234)),
            num_collect::constants::cbrt, std::cbrt);
    }

    SECTION("cbrt of zero") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0.0)),
            num_collect::constants::cbrt, std::cbrt);
    }

    SECTION("cbrt of positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234)),
            num_collect::constants::cbrt, std::cbrt);
    }

    SECTION("cbrt of large positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234e+30)),
            num_collect::constants::cbrt, std::cbrt);
    }

    SECTION("cbrt of infinity") {
        constexpr TestType x = std::numeric_limits<TestType>::infinity();
        constexpr TestType value_at_compile_time =
            num_collect::constants::cbrt(x);
        const TestType value_runtime = num_collect::constants::cbrt(x);
        REQUIRE(std::isinf(value_at_compile_time));
        REQUIRE(value_at_compile_time > num_collect::constants::zero<TestType>);
        REQUIRE(std::isinf(value_runtime));
        REQUIRE(value_runtime > num_collect::constants::zero<TestType>);
    }
}

TEMPLATE_TEST_CASE(
    "num_collect::constants::cbrt (integers)", "", int, long long) {
    SECTION("cbrt of negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-5)),
            num_collect::constants::cbrt, std::cbrt);
    }

    SECTION("cbrt of zero") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0)),
            num_collect::constants::cbrt, std::cbrt);
    }

    SECTION("cbrt of positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(10)),
            num_collect::constants::cbrt, std::cbrt);
    }
}
