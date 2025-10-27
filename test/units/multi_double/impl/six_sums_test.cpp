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
 * \brief Test of functions to add six double numbers.
 */
#include "num_collect/multi_double/impl/six_sums.h"

#include <cstdlib>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::multi_double::impl::six_to_three_sum") {
    using num_collect::multi_double::impl::six_to_three_sum;

    SECTION("calculate") {
        constexpr double x1 = 0x1.082ae5e7d85e0p-24;
        constexpr double x2 = 0x1.f49f008c2be61p+17;
        constexpr double x3 = -0x1.f2485186c2d38p+20;
        constexpr double x4 = -0x1.2138f12941b7bp+14;
        constexpr double x5 = 0x1.81a6c66f2e995p-44;
        constexpr double x6 = 0x1.21b91cdef9d31p+32;

        const auto [r0, r1, r2] = six_to_three_sum(x1, x2, x3, x4, x5, x6);

        CHECK_THAT(r0, Catch::Matchers::WithinRel(x1 + x2 + x3 + x4 + x5 + x6));
        CHECK(std::abs(r1) <=
            std::abs(r0) * std::numeric_limits<double>::epsilon() * 8.0);
        CHECK(std::abs(r2) <= std::abs(r0) *
                std::numeric_limits<double>::epsilon() *
                std::numeric_limits<double>::epsilon() * 8.0);
    }
}
