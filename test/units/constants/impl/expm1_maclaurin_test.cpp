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
 * \brief Test of expm1_maclaurin function.
 */
#include "num_collect/constants/impl/expm1_maclaurin.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::impl::expm1_maclaurin", "", float, double) {
    SECTION("x = 0") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr auto val = num_collect::constants::impl::expm1_maclaurin(x);
        const auto reference = std::expm1(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = 0.2") {
        constexpr auto x = static_cast<TestType>(0.2);
        constexpr auto val = num_collect::constants::impl::expm1_maclaurin(x);
        const auto reference = std::expm1(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = 0.5") {
        constexpr auto x = static_cast<TestType>(0.5);
        constexpr auto val = num_collect::constants::impl::expm1_maclaurin(x);
        const auto reference = std::expm1(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("x = 1") {
        constexpr auto x = static_cast<TestType>(1);
        constexpr auto val = num_collect::constants::impl::expm1_maclaurin(x);
        const auto reference = std::expm1(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}
