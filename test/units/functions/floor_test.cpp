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
 * \brief Test of floor function.
 */
#include "num_collect/functions/floor.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "check_constexpr_function.h"

TEMPLATE_TEST_CASE(
    "num_collect::functions::impl::floor_at_compile_time", "", float, double) {
    SECTION("x = -2") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-2.0)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = -1.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-1.5)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = -1") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-1.0)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = -0.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-0.5)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = 0") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(0.0)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = 0.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(0.5)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = 1.0") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(1.0)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = 1.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(1.5)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }

    SECTION("x = 2.0") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(2.0)),
            num_collect::functions::impl::floor_at_compile_time, std::floor);
    }
}

TEMPLATE_TEST_CASE("num_collect::functions::floor", "", float, double) {
    SECTION("x = -2") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-2.0)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = -1.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-1.5)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = -1") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-1.0)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = -0.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(-0.5)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = 0") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(0.0)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = 0.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(0.5)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = 1.0") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(1.0)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = 1.5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(1.5)),
            num_collect::functions::floor, std::floor);
    }

    SECTION("x = 2.0") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<TestType>(2.0)),
            num_collect::functions::floor, std::floor);
    }
}
