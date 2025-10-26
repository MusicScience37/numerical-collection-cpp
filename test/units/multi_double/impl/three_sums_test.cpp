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
 * \brief Tests of functions to add three double numbers.
 */
#include "num_collect/multi_double/impl/three_sums.h"

#include <cstdlib>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::multi_double::impl::three_to_three_sum") {
    using num_collect::multi_double::impl::three_to_three_sum;

    SECTION("calculate") {
        constexpr double x = 0x1.236079bd56d57p+7;
        constexpr double y = 0x1.041db7585e1f3p+11;
        constexpr double z = 0x1.4c151b290287bp-18;

        const auto [r0, r1, r2] = three_to_three_sum(x, y, z);

        CHECK_THAT(r0, Catch::Matchers::WithinRel(x + y + z));
        CHECK(std::abs(r1) <=
            std::abs(r0) * std::numeric_limits<double>::epsilon() * 8.0);
        CHECK(std::abs(r2) <= std::abs(r0) *
                std::numeric_limits<double>::epsilon() *
                std::numeric_limits<double>::epsilon() * 8.0);
    }
}

TEST_CASE("num_collect::multi_double::impl::three_to_two_sum") {
    using num_collect::multi_double::impl::three_to_two_sum;

    SECTION("calculate") {
        constexpr double x = 0x1.236079bd56d57p+7;
        constexpr double y = 0x1.041db7585e1f3p+11;
        constexpr double z = 0x1.4c151b290287bp-18;

        const auto [r0, r1] = three_to_two_sum(x, y, z);

        CHECK_THAT(r0, Catch::Matchers::WithinRel(x + y + z));
        CHECK(std::abs(r1) <=
            std::abs(r0) * std::numeric_limits<double>::epsilon() * 8.0);
    }
}
