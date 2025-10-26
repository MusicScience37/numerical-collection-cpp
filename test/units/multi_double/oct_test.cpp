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
 * \brief Test of oct class.
 */
#include "num_collect/multi_double/oct.h"

#include <array>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/multi_double/quad.h"

TEST_CASE("num_collect::multi_double::oct") {
    using num_collect::multi_double::oct;
    using num_collect::multi_double::quad;

    SECTION("default constructor") {
        const oct num;

        CHECK(num.term(0) == 0.0);
        CHECK(num.term(1) == 0.0);
        CHECK(num.term(2) == 0.0);
        CHECK(num.term(3) == 0.0);
    }

    SECTION("constructor with an array of terms") {
        constexpr auto terms_array = std::array<double, 4>{1.0, 2.0, 3.0, 4.0};

        const oct num(terms_array);

        CHECK(num.term(0) == 1.0);
        CHECK(num.term(1) == 2.0);
        CHECK(num.term(2) == 3.0);
        CHECK(num.term(3) == 4.0);
    }

    SECTION("constructor with terms") {
        const oct num(1.0, 2.0, 3.0, 4.0);

        CHECK(num.term(0) == 1.0);
        CHECK(num.term(1) == 2.0);
        CHECK(num.term(2) == 3.0);
        CHECK(num.term(3) == 4.0);
    }

    SECTION("constructor with double") {
        const oct num(1.5);

        CHECK(num.term(0) == 1.5);
        CHECK(num.term(1) == 0.0);
        CHECK(num.term(2) == 0.0);
        CHECK(num.term(3) == 0.0);
    }

    SECTION("constructor with quad") {
        const quad quad_num(2.5, 1.5);

        const oct num(quad_num);

        CHECK(num.term(0) == 2.5);
        CHECK(num.term(1) == 1.5);
        CHECK(num.term(2) == 0.0);
        CHECK(num.term(3) == 0.0);
    }

    SECTION("negate a number") {
        const oct num(0x1.0p+0, -0x1.0p-53, -0x1.0p-106, 0x1.0p-159);

        const oct neg_num = -num;

        CHECK(neg_num.term(0) == -0x1.0p+0);
        CHECK(neg_num.term(1) == 0x1.0p-53);
        CHECK(neg_num.term(2) == 0x1.0p-106);
        CHECK(neg_num.term(3) == -0x1.0p-159);
    }
}
