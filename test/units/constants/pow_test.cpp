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
 * \brief Test of pow function.
 */
#include "num_collect/constants/pow.h"

#include <cmath>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::pow(Float, Int)", "",
    (std::pair<float, long long>), (std::pair<double, int>)) {
    using Base = typename TestType::first_type;
    using Exp = typename TestType::second_type;

    SECTION("ordinary") {
        constexpr auto base = static_cast<Base>(4.321);
        constexpr auto exp = static_cast<Exp>(3);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("negative exponent") {
        constexpr auto base = static_cast<Base>(4.321);
        constexpr auto exp = static_cast<Exp>(-3);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("low exponent") {
        constexpr auto base = static_cast<Base>(4.321);
        constexpr auto exp = static_cast<Exp>(-30);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("zero exponent") {
        constexpr auto base = static_cast<Base>(4.321);
        constexpr auto exp = static_cast<Exp>(0);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("large exponent") {
        constexpr auto base = static_cast<Base>(4.321);
        constexpr auto exp = static_cast<Exp>(30);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("negative base") {
        constexpr auto base = static_cast<Base>(-4.321);
        constexpr auto exp = static_cast<Exp>(3);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("low base") {
        constexpr auto base = static_cast<Base>(-4.321e+5);
        constexpr auto exp = static_cast<Exp>(3);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("large base") {
        constexpr auto base = static_cast<Base>(4.321e+5);
        constexpr auto exp = static_cast<Exp>(3);
        constexpr Base val = num_collect::constants::pow(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::pow(Float, Float)", "", float, double) {
    SECTION("ordinary") {
        constexpr auto base = static_cast<TestType>(4.321);
        constexpr auto exp = static_cast<TestType>(1.234);
        constexpr TestType val = num_collect::constants::pow(base, exp);
        const TestType reference = std::pow(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("negative exponent") {
        constexpr auto base = static_cast<TestType>(4.321);
        constexpr auto exp = static_cast<TestType>(-2.345);
        constexpr TestType val = num_collect::constants::pow(base, exp);
        const TestType reference = std::pow(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("low exponent") {
        constexpr auto base = static_cast<TestType>(4.321);
        constexpr auto exp = static_cast<TestType>(-123.456);
        constexpr TestType val = num_collect::constants::pow(base, exp);
        const TestType reference = std::pow(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("zero exponent") {
        constexpr auto base = static_cast<TestType>(4.321);
        constexpr auto exp = static_cast<TestType>(0);
        constexpr TestType val = num_collect::constants::pow(base, exp);
        const TestType reference = std::pow(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("large exponent") {
        constexpr auto base = static_cast<TestType>(4.321);
        constexpr auto exp = static_cast<TestType>(123.456);
        constexpr TestType val = num_collect::constants::pow(base, exp);
        const TestType reference = std::pow(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("small base") {
        constexpr auto base = static_cast<TestType>(4.321e-10);
        constexpr auto exp = static_cast<TestType>(1.234);
        constexpr TestType val = num_collect::constants::pow(base, exp);
        const TestType reference = std::pow(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("large base") {
        constexpr auto base = static_cast<TestType>(4.321e+10);
        constexpr auto exp = static_cast<TestType>(1.234);
        constexpr TestType val = num_collect::constants::pow(base, exp);
        const TestType reference = std::pow(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}
