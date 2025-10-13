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
 * \brief Test of format_decimal_scientific_to function for quad class.
 */
#include "num_collect/multi_double/impl/quad_format_decimal_scientific_to.h"

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <fmt/format.h>

#include "num_collect/multi_double/quad.h"

TEST_CASE("num_collect::multi_double::impl::format_decimal_scientific_to") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::format_decimal_scientific_to;

    SECTION("format a positive small number") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.13ef763c41377p-32, -0x1.f5dcebaf97254p-86);
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str,
            Catch::Matchers::Matches(
                R"(2\.50961782402166541591647970941\de\-10)"));
    }

    SECTION("format a positive large number") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.9d7e70c5a3f20p+5, -0x1.f382e6eae8b68p-50);
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str,
            Catch::Matchers::Matches(
                R"(5\.16867385330826874401422831613\de\+01)"));
    }

    SECTION("format a negative small number") {
        constexpr auto value =  // cspell: disable-next-line
            quad(-0x1.6e10aacf4341fp-5, -0x1.b639576b7c2d4p-59);
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str,
            Catch::Matchers::Matches(
                R"(-4\.46856819272414234330914287297\de\-02)"));
    }

    SECTION("format a negative large number") {
        constexpr auto value =  // cspell: disable-next-line
            quad(-0x1.3dd99e6fee216p+47, -0x1.bf4ade823c824p-7);
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str,
            Catch::Matchers::Matches(
                R"(-1\.74739926021904701150282523006\de\+14)"));
    }

    SECTION("format zero") {
        constexpr auto value = quad(0.0);
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str,
            Catch::Matchers::Matches(
                R"(0\.000000000000000000000000000000e\+00)"));
    }

    SECTION("format a negative large number with the maximum precision") {
        constexpr auto value =  // cspell: disable-next-line
            quad(-0x1.3dd99e6fee216p+47, -0x1.bf4ade823c824p-7);
        constexpr int precision = 35;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str,
            Catch::Matchers::Matches(
                R"(-1\.74739926021904701150282523006\d\d\d\d\d\de\+14)"));
    }

    SECTION("format a positive number with a small precision") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.2c7e2d0f0b930p-37, -0x1.c1e58d19f20a4p-91);
        constexpr int precision = 10;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str, Catch::Matchers::Matches(R"(8\.540521180\de-12)"));
    }

    SECTION("format a negative number with a small precision") {
        constexpr auto value =  // cspell: disable-next-line
            quad(-0x1.0837355ab7117p+20, 0x1.e92e08a2b49f0p-34);
        constexpr int precision = 10;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str, Catch::Matchers::Matches(R"(-1\.082227334\de\+06)"));
    }

    SECTION("format zero with a small precision") {
        constexpr auto value = quad(0.0);
        constexpr int precision = 10;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK_THAT(str, Catch::Matchers::Matches(R"(0\.0000000000e\+00)"));
    }

    SECTION("format NaN") {
        constexpr auto value = quad(std::numeric_limits<double>::quiet_NaN());
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK(str == "nan");
    }

    SECTION("format +infinity") {
        constexpr auto value = quad(std::numeric_limits<double>::infinity());
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK(str == "inf");
    }

    SECTION("format -infinity") {
        constexpr auto value = quad(-std::numeric_limits<double>::infinity());
        constexpr int precision = 30;

        std::string str;
        format_decimal_scientific_to(std::back_inserter(str), value, precision);

        CHECK(str == "-inf");
    }
}
