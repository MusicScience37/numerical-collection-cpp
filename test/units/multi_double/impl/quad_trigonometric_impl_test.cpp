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
 * \brief Test of internal functions to calculate trigonometric functions
 * for quad class.
 */
#include "num_collect/multi_double/impl/quad_trigonometric_impl.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../format_quad_for_test.h"  // IWYU pragma: keep
#include "../quad_approx.h"

TEST_CASE("num_collect::multi_double::impl::sin_maclaurin") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::sin_maclaurin;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.921fb54442d18p-1, -0x1.1a62633145c08p-55),
                quad(-0x1.6a09e667f3bcdp-1, 0x1.bdd3413b26455p-55)),
            std::make_tuple(quad(-0x1.cc12b01efdd2fp-17, 0x1.8fb1ce29f6c98p-71),
                quad(-0x1.cc12b01ebfe8fp-17, 0x1.0430f48c838c7p-71)),
            std::make_tuple(quad(0x1.4918d08cf7f07p-25, 0x1.942dc62504670p-80),
                quad(0x1.4918d08cf7f06p-25, -0x1.c07006a6d6621p-80)),
            std::make_tuple(quad(0x1.921fb54442d18p-1, 0x1.1a62633145c08p-55),
                quad(0x1.6a09e667f3bcdp-1, -0x1.bdd3413b26455p-55)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = sin_maclaurin(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(0.0);

        const quad actual = sin_maclaurin(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::impl::cos_maclaurin") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::cos_maclaurin;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.921fb54442d18p-1, -0x1.1a62633145c08p-55),
                quad(0x1.6a09e667f3bcdp-1, -0x1.bdd3413b26456p-55)),
            std::make_tuple(quad(-0x1.cc12b01efdd2fp-17, 0x1.8fb1ce29f6c98p-71),
                quad(0x1.ffffffff314b3p-1, 0x1.5b399ee321707p-55)),
            std::make_tuple(quad(0x1.4918d08cf7f07p-25, 0x1.942dc62504670p-80),
                quad(0x1.ffffffffffff9p-1, 0x1.8ef358de66429p-55)),
            std::make_tuple(quad(0x1.921fb54442d18p-1, 0x1.1a62633145c08p-55),
                quad(0x1.6a09e667f3bcdp-1, -0x1.bdd3413b26456p-55)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = cos_maclaurin(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(1.0);

        const quad actual = cos_maclaurin(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}
