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
 * \brief Test of pow_pos_int function.
 */
#include "num_collect/constants/impl/pow_pos_int.h"

#include <cmath>
#include <ostream>
#include <string>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::impl::pow_pos_int", "",
    (std::pair<float, int>), (std::pair<double, unsigned int>)) {
    using Base = typename TestType::first_type;
    using Exp = typename TestType::second_type;

    SECTION("for exponent = 0") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(0);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        REQUIRE(val == static_cast<Base>(1));
    }

    SECTION("for exponent = 1") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(1);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(base));
    }

    SECTION("for exponent = 2") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(2);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("for exponent = 3") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(3);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("for exponent = 4") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(4);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("for exponent = 5") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(5);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("for exponent = 6") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(6);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("for exponent = 10") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(10);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("for exponent = 100") {
        constexpr auto base = static_cast<Base>(1.234);
        constexpr auto exp = static_cast<Exp>(100);
        constexpr Base val =
            num_collect::constants::impl::pow_pos_int(base, exp);
        const Base reference = std::pow(base, static_cast<Base>(exp));
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}
