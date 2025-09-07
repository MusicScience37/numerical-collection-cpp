/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of double_factorial function.
 */
#include "num_collect/constants/double_factorial.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE_SIG("num_collect::constants::double_factorial", "",
    ((typename Result, typename Integer), Result, Integer),
    // Integer to integer.
    (int, int), (unsigned int, unsigned int),
    // Integer to floating-point number.
    (double, int), (double, unsigned int)) {
    SECTION("n = 0") {
        constexpr auto n = static_cast<Integer>(0);
        constexpr auto val =
            num_collect::constants::double_factorial<Result>(n);
        constexpr auto true_val = static_cast<Result>(1);
        STATIC_REQUIRE(val == true_val);
    }

    SECTION("n = 1") {
        constexpr auto n = static_cast<Integer>(1);
        constexpr auto val =
            num_collect::constants::double_factorial<Result>(n);
        constexpr auto true_val = static_cast<Result>(1);
        STATIC_REQUIRE(val == true_val);
    }

    SECTION("n = 2") {
        constexpr auto n = static_cast<Integer>(2);
        constexpr auto val =
            num_collect::constants::double_factorial<Result>(n);
        constexpr auto true_val = static_cast<Result>(2);
        STATIC_REQUIRE(val == true_val);
    }

    SECTION("n = 3") {
        constexpr auto n = static_cast<Integer>(3);
        constexpr auto val =
            num_collect::constants::double_factorial<Result>(n);
        constexpr auto true_val = static_cast<Result>(3);
        STATIC_REQUIRE(val == true_val);
    }

    SECTION("n = 4") {
        constexpr auto n = static_cast<Integer>(4);
        constexpr auto val =
            num_collect::constants::double_factorial<Result>(n);
        constexpr auto true_val = static_cast<Result>(8);
        STATIC_REQUIRE(val == true_val);
    }

    SECTION("n = 5") {
        constexpr auto n = static_cast<Integer>(5);
        constexpr auto val =
            num_collect::constants::double_factorial<Result>(n);
        constexpr auto true_val = static_cast<Result>(15);
        STATIC_REQUIRE(val == true_val);
    }

    SECTION("n = -1") {
        if constexpr (std::signed_integral<Integer>) {
            constexpr auto n = static_cast<Integer>(-1);
            constexpr auto val =
                num_collect::constants::double_factorial<Result>(n);
            if constexpr (std::floating_point<Result>) {
                REQUIRE(std::isnan(val));
            } else {
                constexpr auto true_val = static_cast<Result>(0);
                STATIC_REQUIRE(val == true_val);
            }
        }
    }
}
