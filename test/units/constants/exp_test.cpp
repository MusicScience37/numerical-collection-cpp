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
 * \brief Test of exp function.
 */
#include "num_collect/constants/exp.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::impl::exp_at_compile_time", "", float, double) {
    SECTION("negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-1.234)),
            num_collect::constants::impl::exp_at_compile_time, std::exp);
    }

    SECTION("low negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-50.123)),
            num_collect::constants::impl::exp_at_compile_time, std::exp);
    }

    SECTION("zero") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.0)),
            num_collect::constants::impl::exp_at_compile_time, std::exp);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234)),
            num_collect::constants::impl::exp_at_compile_time, std::exp);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(50.123)),
            num_collect::constants::impl::exp_at_compile_time, std::exp);
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::exp", "", float, double) {
    SECTION("negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-1.234)),
            num_collect::constants::exp, std::exp);
    }

    SECTION("low negative number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(-50.123)),
            num_collect::constants::exp, std::exp);
    }

    SECTION("zero") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.0)),
            num_collect::constants::exp, std::exp);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(1.234)),
            num_collect::constants::exp, std::exp);
    }

    SECTION("positive number") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(50.123)),
            num_collect::constants::exp, std::exp);
    }
}
