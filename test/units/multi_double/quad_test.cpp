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
 * \brief test of quad class
 */
#include "num_collect/multi_double/quad.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::multi_double::quad") {
    using num_collect::multi_double::quad;

    SECTION("construct without arguments") {
        quad num;
        REQUIRE_THAT(num.high(), Catch::Matchers::WithinULP(0.0, 0));
        REQUIRE_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    SECTION("construct with double argument") {
        constexpr double value = 3.14;
        quad num = value;
        REQUIRE_THAT(num.high(), Catch::Matchers::WithinULP(value, 0));
        REQUIRE_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    SECTION("construct with int argument") {
        constexpr int value = 37;
        quad num = value;
        REQUIRE_THAT(num.high(),
            Catch::Matchers::WithinULP(static_cast<double>(value), 0));
        REQUIRE_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    SECTION("add a quad number with operator+= (1)") {
        constexpr auto a = quad(0x1.0000000000001p+0, 0x0.7000000000001p-52);
        constexpr auto b = quad(0x1.0000000000007p+0, 0x0.8000000000002p-52);
        constexpr auto sum_true =
            quad(0x2.0000000000008p+0, 0x0.F000000000003p-52);
        auto sum = a;
        sum += b;
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(sum.low(), Catch::Matchers::WithinULP(sum_true.low(), 0));
    }

    SECTION("add a quad number with operator+= (2)") {
        constexpr auto a = quad(0x1.0000000000000p+0, 0x0.7p-52);
        constexpr auto b = quad(0x0.8000000000000p+0, 0x0.2p-52);
        constexpr auto sum_true = quad(0x1.8000000000001p+0, -0x0.7p-52);
        auto sum = a;
        sum += b;
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(sum.low(), Catch::Matchers::WithinULP(sum_true.low(), 0));
    }

    SECTION("add a quad number with operator+= (3)") {
        constexpr auto a = quad(0x1.0p+0, 0x0.7p-52);
        constexpr auto b = quad(-0x1.0p+0, 0x0.2p-52);
        constexpr auto sum_true = quad(0x0.9p-52);
        auto sum = a;
        sum += b;
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(sum.low(), Catch::Matchers::WithinULP(sum_true.low(), 0));
    }

    SECTION("subtract a quad number with operator-=") {
        constexpr auto a = quad(0x1.0000000000001p+0, 0x0.7000000000001p-52);
        constexpr auto b = quad(-0x1.0000000000007p+0, -0x0.8000000000002p-52);
        constexpr auto dif_true =
            quad(0x2.0000000000008p+0, 0x0.F000000000003p-52);
        auto dif = a;
        dif -= b;
        REQUIRE_THAT(
            dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        REQUIRE_THAT(dif.low(), Catch::Matchers::WithinULP(dif_true.low(), 0));
    }

    SECTION("multiply quad numbers with operator*= (1)") {
        constexpr auto a = quad(0x1.0p+1, 0x0.1p-51);
        constexpr auto b = quad(0x1.0p+2, 0x0.2p-50);
        constexpr auto prod_true = quad(0x1.0p+3, 0x0.3p-49);
        auto prod = a;
        prod *= b;
        REQUIRE_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        REQUIRE_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), 0));
    }

    SECTION("multiply quad numbers with operator*= (2)") {
        constexpr auto a = quad(0x1.0000001p+1);
        constexpr auto b = quad(0x1.0000001p+2);
        constexpr auto prod_true = quad(0x1.0000002p+3, 0x0.1p-49);
        auto prod = a;
        prod *= b;
        REQUIRE_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        REQUIRE_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), 0));
    }
}
