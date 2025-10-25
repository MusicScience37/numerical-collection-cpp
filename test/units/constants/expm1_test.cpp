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
 * \brief Test of expm1 function.
 */
#include "num_collect/constants/expm1.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

TEMPLATE_TEST_CASE(
    "num_collect::constants::impl::expm1_at_compile_time", "", float, double) {
    SECTION("negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.123)),
            num_collect::constants::impl::expm1_at_compile_time, std::expm1);
    }

    SECTION("small negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.123e-5)),
            num_collect::constants::impl::expm1_at_compile_time, std::expm1);
    }

    SECTION("low negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-50.123)),
            num_collect::constants::impl::expm1_at_compile_time, std::expm1);
    }

    SECTION("zero") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.0)),
            num_collect::constants::impl::expm1_at_compile_time, std::expm1);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.123)),
            num_collect::constants::impl::expm1_at_compile_time, std::expm1);
    }

    SECTION("small positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.123e-5)),
            num_collect::constants::impl::expm1_at_compile_time, std::expm1);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(50.123)),
            num_collect::constants::impl::expm1_at_compile_time, std::expm1);
    }
}

TEMPLATE_TEST_CASE("num_collect::constants::expm1", "", float, double) {
    SECTION("negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.123)),
            num_collect::constants::expm1, std::expm1);
    }

    SECTION("small negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-0.123e-5)),
            num_collect::constants::expm1, std::expm1);
    }

    SECTION("low negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-50.123)),
            num_collect::constants::expm1, std::expm1);
    }

    SECTION("zero") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.0)),
            num_collect::constants::expm1, std::expm1);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.123)),
            num_collect::constants::expm1, std::expm1);
    }

    SECTION("small positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.123e-5)),
            num_collect::constants::expm1, std::expm1);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(50.123)),
            num_collect::constants::expm1, std::expm1);
    }
}
