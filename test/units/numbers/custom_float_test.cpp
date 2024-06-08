/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance using the License.
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
 * \brief Test of custom_float class.
 */
#include "num_collect/numbers/custom_float.h"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::numbers::custom_float") {
    using num_collect::numbers::custom_float;

    constexpr num_collect::index_type radix = 7;
    using test_type = custom_float<radix>;

    SECTION("copy and move") {
        STATIC_REQUIRE(std::is_copy_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_copy_assignable_v<test_type>);
        STATIC_REQUIRE(std::is_move_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_move_assignable_v<test_type>);
    }

    SECTION("construct") {
        const auto num = test_type();

        REQUIRE(num.lowest_ind() == 0);
        REQUIRE(num.highest_ind() == 0);
        REQUIRE(num.at(0) == 0);
    }

    SECTION("access digits using at") {
        const auto num = test_type();

        REQUIRE(num.at(0) == 0);
        REQUIRE_THROWS(num.at(1));
        REQUIRE_THROWS(num.at(-1));
    }

    SECTION("access digits using operator[]") {
        const auto num = test_type();

        REQUIRE(num[0] == 0);
    }

    SECTION("create digits using operator[]") {
        auto num = test_type();

        constexpr typename test_type::digit_type digit2 = 5;
        REQUIRE_NOTHROW(num[2] = digit2);

        REQUIRE(num.lowest_ind() == 0);
        REQUIRE(num.highest_ind() == 2);
        REQUIRE(num.at(0) == 0);
        REQUIRE(num.at(1) == 0);
        REQUIRE(num.at(2) == digit2);
    }

    SECTION("create digits using push_to_lowest") {
        auto num = test_type();

        constexpr typename test_type::digit_type digit1 = 5;
        REQUIRE_NOTHROW(num.push_to_lowest(digit1));

        REQUIRE(num.lowest_ind() == -1);
        REQUIRE(num.highest_ind() == 0);
        REQUIRE(num.at(0) == 0);
        REQUIRE(num.at(-1) == digit1);
    }

    SECTION("create digits using push_to_highest") {
        auto num = test_type();

        constexpr typename test_type::digit_type digit1 = 5;
        REQUIRE_NOTHROW(num.push_to_highest(digit1));

        REQUIRE(num.lowest_ind() == 0);
        REQUIRE(num.highest_ind() == 1);
        REQUIRE(num.at(0) == 0);
        REQUIRE(num.at(1) == digit1);
    }

    SECTION("move digits") {
        auto num = test_type();
        num[3] = 7;                       // NOLINT
        num[4] = 11;                      // NOLINT
        num[5] = 13;                      // NOLINT
        REQUIRE(num.lowest_ind() == 0);   // NOLINT
        REQUIRE(num.highest_ind() == 5);  // NOLINT

        REQUIRE_NOTHROW(num.move_digits(-2));

        REQUIRE(num.lowest_ind() == -2);  // NOLINT
        REQUIRE(num.highest_ind() == 3);  // NOLINT
        REQUIRE(num.at(1) == 7);          // NOLINT
        REQUIRE(num.at(2) == 11);         // NOLINT
        REQUIRE(num.at(3) == 13);         // NOLINT
    }

    SECTION("change the range of digits") {
        auto num = test_type();
        num[3] = 7;                       // NOLINT
        num[4] = 11;                      // NOLINT
        num[5] = 13;                      // NOLINT
        REQUIRE(num.lowest_ind() == 0);   // NOLINT
        REQUIRE(num.highest_ind() == 5);  // NOLINT

        REQUIRE_NOTHROW(num.resize(-1, 4));  // NOLINT
        REQUIRE(num.lowest_ind() == -1);     // NOLINT
        REQUIRE(num.highest_ind() == 4);     // NOLINT
        REQUIRE(num.at(-1) == 0);            // NOLINT
        REQUIRE(num.at(0) == 0);             // NOLINT
        REQUIRE(num.at(1) == 0);             // NOLINT
        REQUIRE(num.at(2) == 0);             // NOLINT
        REQUIRE(num.at(3) == 7);             // NOLINT
        REQUIRE(num.at(4) == 11);            // NOLINT
    }
}
