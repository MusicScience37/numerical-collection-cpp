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
 * \brief Test of mathematical functions of quad class.
 */
#include "num_collect/multi_double/quad_math.h"

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "format_quad_for_test.h"  // IWYU pragma: keep
#include "quad_approx.h"

TEST_CASE("num_collect::multi_double::abs") {
    using num_collect::multi_double::abs;
    using num_collect::multi_double::quad;

    SECTION("calculate") {
        CHECK(abs(quad(std::numeric_limits<double>::min())) ==
            quad(std::numeric_limits<double>::min()));
        CHECK(abs(quad(0.0, std::numeric_limits<double>::min())) ==
            quad(0.0, std::numeric_limits<double>::min()));
        CHECK(abs(quad(0.0, 0.0)) == quad(0.0, 0.0));
        CHECK(abs(quad(0.0, -std::numeric_limits<double>::min())) ==
            quad(0.0, std::numeric_limits<double>::min()));
        CHECK(abs(quad(-std::numeric_limits<double>::min())) ==
            quad(std::numeric_limits<double>::min()));
    }
}

TEST_CASE("num_collect::multi_double::sqrt") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::sqrt;

    SECTION("calculate for positive values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.0000000000000p+0, 0x0.0p+0),
                quad(0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.0000000000000p+1, 0x0.0p+0),
                quad(0x1.6a09e667f3bcdp+0, -0x1.bdd3413b26456p-54)),
            std::make_tuple(quad(0x1.1a651c97cd404p-22, -0x1.dfa323ed68808p-76),
                quad(0x1.0cdfb04deed57p-11, -0x1.dea00ee468c8fp-68)),
            std::make_tuple(quad(0x1.10c1e666a56e0p+0, -0x1.f42263d9ec5c0p-57),
                quad(0x1.083ef3d717c03p+0, 0x1.d4c42dba4b1a5p-54)),
            std::make_tuple(quad(0x1.2c6adf012f63ap+27, 0x1.7f94bdac919b8p-27),
                quad(0x1.8830e22df76a4p+13, -0x1.621a2c2e4f071p-41))
            // cspell: enable
            // NOLINTEND
        }));

        const quad actual = sqrt(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(
            actual, make_quad_within_rel_matcher(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0, 0.0);
        const quad expected(0.0, 0.0);

        const quad actual = sqrt(input);

        CHECK(actual == expected);
    }
}
