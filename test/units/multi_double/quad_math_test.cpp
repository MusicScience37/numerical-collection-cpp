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
        INFO("input: " << format_quad_for_test(input));

        const quad actual = sqrt(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0, 0.0);
        const quad expected(0.0, 0.0);

        const quad actual = sqrt(input);

        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::exp") {
    using num_collect::multi_double::exp;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.6180000000000p+9, 0x0.0p+0),
                quad(0x1.029ade2342558p-1020, -0x0.0000000000001p-1022)),
            std::make_tuple(quad(-0x1.eecd4d2a8802bp+8, -0x1.0c0bd235ca6d4p-46),
                quad(0x1.1c60dd15c954ep-714, -0x1.2a61ff0c2188dp-770)),
            std::make_tuple(quad(-0x1.b93d3796001a0p+4, -0x1.ad3954413d8ccp-50),
                quad(0x1.28f7677c1504fp-40, 0x1.5c549fa1512ccp-94)),
            std::make_tuple(quad(-0x1.b684eabe415bbp+0, 0x1.8394ecb303f2cp-54),
                quad(0x1.7150f607b9017p-3, 0x1.73d959ef6441cp-60)),
            std::make_tuple(quad(-0x1.61e4f765fd8aep-10, 0x1.5182a9930be0cp-64),
                quad(0x1.ff4f2c145b838p-1, 0x1.fb337f2e60601p-57)),
            std::make_tuple(quad(-0x1.fd71823e9ed31p-28, 0x1.456b15bb2ec98p-83),
                quad(0x1.ffffffc051cfcp-1, -0x1.ef06e7cbb5fbcp-56)),
            std::make_tuple(quad(0x1.3e3e59d300f44p-40, 0x1.c41897769fec4p-94),
                quad(0x1.00000000013e4p+0, -0x1.a62cfe45e9747p-56)),
            std::make_tuple(quad(0x1.61e4f765fd8aep-10, -0x1.5182a9930be0cp-64),
                quad(0x1.00588889667d3p+0, 0x1.8740c1fa8e87cp-54)),
            std::make_tuple(quad(0x1.e37bed2c3aa0bp+0, -0x1.e2d5f1238d4c0p-56),
                quad(0x1.a70d100d10754p+2, 0x1.fa696398ba718p-52)),
            std::make_tuple(quad(0x1.5832e2e5d966bp+5, 0x1.8794e6d1f4d78p-49),
                quad(0x1.0d0ced69dd7b7p+62, -0x1.271cd9a4320d3p+8)),
            std::make_tuple(quad(0x1.15d8c5d3e4004p+9, -0x1.01f96f5f98c28p-45),
                quad(0x1.9ecf9ee00e06bp+801, 0x1.8ebd36d627d80p+747)),
            std::make_tuple(quad(0x1.6280000000000p+9, 0x0.0p+0),
                quad(0x1.d422d2be5dc9bp+1022, -0x1.916aa7a2c8d07p+967)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = exp(input);
#ifdef _MSC_VER
        // MSVC seems to have lower accuracy.
        constexpr quad relative_tolerance(0x1.0p-90);
#else
        constexpr quad relative_tolerance(0x1.0p-97);
#endif
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(1.0);

        const quad actual = exp(input);

        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::expm1") {
    using num_collect::multi_double::expm1;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.6180000000000p+9, 0x0.0p+0),
                quad(-0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.eecd4d2a8802bp+8, -0x1.0c0bd235ca6d4p-46),
                quad(-0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.b93d3796001a0p+4, -0x1.ad3954413d8ccp-50),
                quad(-0x1.fffffffffdae1p-1, -0x1.3107d5f6151d6p-57)),
            std::make_tuple(quad(-0x1.b684eabe415bbp+0, 0x1.8394ecb303f2cp-54),
                quad(-0x1.a3abc27e11bfap-1, -0x1.e8c26a6109bbep-56)),
            std::make_tuple(quad(-0x1.61e4f765fd8aep-10, 0x1.5182a9930be0cp-64),
                quad(-0x1.61a7d748f8fc1p-10, 0x1.99bf973030078p-64)),
            std::make_tuple(quad(-0x1.fd71823e9ed31p-28, 0x1.456b15bb2ec98p-83),
                quad(-0x1.fd71821ef06e8p-28, 0x1.a25022221087cp-83)),
            std::make_tuple(quad(0x1.3e3e59d300f44p-40, 0x1.c41897769fec4p-94),
                quad(0x1.3e3e59d301ba1p-40, 0x1.a2e3928e1558cp-94)),
            std::make_tuple(quad(0x1.61e4f765fd8aep-10, -0x1.5182a9930be0cp-64),
                quad(0x1.62222599f4d87p-10, 0x1.0307ea3a1eec9p-64)),
            std::make_tuple(quad(0x1.e37bed2c3aa0bp+0, -0x1.e2d5f1238d4c0p-56),
                quad(0x1.670d100d10754p+2, 0x1.fa696398ba718p-52)),
            std::make_tuple(quad(0x1.5832e2e5d966bp+5, 0x1.8794e6d1f4d78p-49),
                quad(0x1.0d0ced69dd7b7p+62, -0x1.281cd9a4320d3p+8)),
            std::make_tuple(quad(0x1.15d8c5d3e4004p+9, -0x1.01f96f5f98c28p-45),
                quad(0x1.9ecf9ee00e06bp+801, 0x1.8ebd36d627d80p+747)),
            std::make_tuple(quad(0x1.6280000000000p+9, 0x0.0p+0),
                quad(0x1.d422d2be5dc9bp+1022, -0x1.916aa7a2c8d07p+967)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = expm1(input);
#ifdef _MSC_VER
        // MSVC seems to have lower accuracy.
        constexpr quad relative_tolerance(0x1.0p-90);
#else
        constexpr quad relative_tolerance(0x1.0p-97);
#endif
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(0.0);

        const quad actual = expm1(input);

        CHECK(actual == expected);
    }
}
