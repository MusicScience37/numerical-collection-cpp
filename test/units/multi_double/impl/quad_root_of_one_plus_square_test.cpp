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
 * \brief Test of root_of_one_plus_square function.
 */
#include "num_collect/multi_double/impl/quad_root_of_one_plus_square.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../format_quad_for_test.h"  // IWYU pragma: keep
#include "../quad_approx.h"

TEST_CASE("num_collect::multi_double::impl::root_of_one_plus_square") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::root_of_one_plus_square;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.766868f7c0304p-25, -0x1.f8e07514ac7d8p-80),
                quad(0x1.0000000000004p+0, 0x1.1ca9ea4acf185p-54)),
            std::make_tuple(quad(0x1.05f644428982cp+4, -0x1.8e5f1b1bdb518p-51),
                quad(0x1.06733caf8fce5p+4, 0x1.e02ce384accf3p-52)),
            std::make_tuple(quad(0x1.aead8b8a75c89p+10, 0x1.a966b8ab353d0p-44),
                quad(0x1.aead904bd0b13p+10, 0x1.31c45aeae1c96p-46)),
            std::make_tuple(quad(0x1.4470bceaffb8dp+44, -0x1.33fdd08dd5560p-11),
                quad(0x1.4470bceaffb8dp+44, -0x1.33fdd08da2d63p-11)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = root_of_one_plus_square(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(1.0);

        const quad actual = root_of_one_plus_square(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}
