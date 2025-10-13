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
 * \brief Test of internal functions to calculate pow function of quad
 * class.
 */
#include "num_collect/multi_double/impl/quad_pow_impl.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../format_quad_for_test.h"  // IWYU pragma: keep
#include "../quad_approx.h"

TEST_CASE("num_collect::multi_double::impl::pow_general_impl") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::pow_general_impl;

    SECTION("calculate for non-zero values") {
        quad base;
        quad exponent;
        quad expected;
        std::tie(base, exponent,
            expected) = GENERATE(Catch::Generators::table<quad, quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.be580cb8d6672p+5, -0x1.7c1b0862537e4p-49),
                quad(0x1.7ff89514e4ba3p+2, 0x1.d9043a8ea4840p-55),
                quad(0x1.c0a64f3223e64p+34, 0x1.7e91903471238p-20)),
            std::make_tuple(quad(0x1.5a2b98fe9585cp-10, -0x1.0c36fa4e937a0p-66),
                quad(0x1.377eb204dfb45p+2, 0x1.61c693b9f146cp-52),
                quad(0x1.5d29dca66ad6bp-47, 0x1.0abe488f081a0p-103)),
            std::make_tuple(quad(0x1.abf6487b5305fp+5, 0x1.924c526aeb870p-50),
                quad(0x1.5b7d6d2e1b674p-2, 0x1.79c41e9ec3e18p-56),
                quad(0x1.edf9f83e3942fp+1, 0x1.8d17126891f64p-53)),
            std::make_tuple(quad(0x1.5df1c02a3661fp-12, 0x1.98c0818650188p-67),
                quad(-0x1.30a1b401d3ba5p+3, 0x1.c85f223c97498p-52),
                quad(0x1.ec627b697f044p+109, 0x1.4e4e95fa15f0cp+55)),
            std::make_tuple(quad(0x1.17ce6ac42961cp-8, -0x1.070ccd508d778p-63),
                quad(-0x1.1f55dcd10d285p-3, -0x1.2c2ce0b511ca4p-57),
                quad(0x1.13368fce32b95p+1, 0x1.8f8f57d27c7b8p-53)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("base: " << format_quad_for_test(base));
        INFO("exponent: " << format_quad_for_test(exponent));

        const quad actual = pow_general_impl(base, exponent);
        constexpr quad relative_tolerance(0x1.0p-94);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}
