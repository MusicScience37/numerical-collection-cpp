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
 * \brief Test of sqrt function.
 */
#include "num_collect/constants/sqrt.h"

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::constants::sqrt") {
    SECTION("sqrt of negative number") {
        constexpr double x = -1.0;
        constexpr double val = num_collect::constants::sqrt(x);
        REQUIRE(std::isnan(val));
    }

    SECTION("sqrt of zero") {
        constexpr double x = 0.0;
        constexpr double val = num_collect::constants::sqrt(x);
        REQUIRE(val == 0.0);
    }

    SECTION("sqrt of positive number") {
        constexpr double true_val = 1.234;
        constexpr double x = true_val * true_val;
        constexpr double val = num_collect::constants::sqrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("sqrt of large positive number") {
        constexpr double true_val = 1.234e+100;
        constexpr double x = true_val * true_val;
        constexpr double val = num_collect::constants::sqrt(x);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
    }

    SECTION("sqrt of infinity") {
        constexpr double x = std::numeric_limits<double>::infinity();
        constexpr double val = num_collect::constants::sqrt(x);
        REQUIRE(std::isinf(val));
        REQUIRE(val > 0.0);
    }
}
