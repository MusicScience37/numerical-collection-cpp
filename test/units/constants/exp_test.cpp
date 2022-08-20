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
#include <ostream>
#include <string>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::exp", "", float, double) {
    SECTION("negative number") {
        constexpr auto x = static_cast<TestType>(-1.234);
        constexpr TestType val = num_collect::constants::exp(x);
        const TestType reference = std::exp(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("low negative number") {
        constexpr auto x = static_cast<TestType>(-50.123);
        constexpr TestType val = num_collect::constants::exp(x);
        const TestType reference = std::exp(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("zero") {
        constexpr auto x = static_cast<TestType>(0.0);
        constexpr TestType val = num_collect::constants::exp(x);
        constexpr auto true_val = static_cast<TestType>(1.0);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("positive number") {
        constexpr auto x = static_cast<TestType>(1.234);
        constexpr TestType val = num_collect::constants::exp(x);
        const TestType reference = std::exp(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("positive number") {
        constexpr auto x = static_cast<TestType>(50.123);
        constexpr TestType val = num_collect::constants::exp(x);
        const TestType reference = std::exp(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}
