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

#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "format_quad_for_test.h"  // IWYU pragma: keep
#include "num_collect/multi_double/impl/quad_internal_constants.h"
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
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = exp(input);
        constexpr quad relative_tolerance(0x1.0p-98);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(1.0);

        const quad actual = exp(input);

        CHECK(actual == expected);
    }

    SECTION("calculate for too large value") {
        const auto input = quad(710.0);

        const auto actual = exp(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(std::isinf(actual.high()));
        CHECK(actual.high() > 0.0);
    }

    SECTION("calculate for too small value") {
        const auto input = quad(-708.0);
        const quad expected(0.0);

        const auto actual = exp(input);

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
            std::make_tuple(quad(-0x1.eecd4d2a8802bp+8, -0x1.0c0bd235ca6d4p-46),
                quad(-0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.b93d3796001a0p+4, -0x1.ad3954413d8ccp-50),
                quad(-0x1.fffffffffdae1p-1, -0x1.3107d5f6151d6p-57)),
            std::make_tuple(quad(-0x1.b684eabe415bbp+0, 0x1.8394ecb303f2cp-54),
                quad(-0x1.a3abc27e11bfap-1, -0x1.e8c26a6109bbep-56)),
            std::make_tuple(
                quad(-0x1.672324c836651p-10, -0x1.b6c3760bf5d78p-64),
                quad(-0x1.66e431b1a3f40p-10, 0x1.311abd9deeb44p-64)),
            std::make_tuple(quad(-0x1.61e4f765fd8aep-10, 0x1.5182a9930be0cp-64),
                quad(-0x1.61a7d748f8fc1p-10, 0x1.99bf973030078p-64)),
            std::make_tuple(quad(-0x1.fd71823e9ed31p-28, 0x1.456b15bb2ec98p-83),
                quad(-0x1.fd71821ef06e8p-28, 0x1.a25022221087cp-83)),
            std::make_tuple(quad(0x1.3e3e59d300f44p-40, 0x1.c41897769fec4p-94),
                quad(0x1.3e3e59d301ba1p-40, 0x1.a2e3928e1558cp-94)),
            std::make_tuple(quad(0x1.61e4f765fd8aep-10, -0x1.5182a9930be0cp-64),
                quad(0x1.62222599f4d87p-10, 0x1.0307ea3a1eec9p-64)),
            std::make_tuple(quad(0x1.672324c836651p-10, 0x1.b6c3760bf5d78p-64),
                quad(0x1.676226986da0ep-10, -0x1.89ff79b779b39p-64)),
            std::make_tuple(quad(0x1.e37bed2c3aa0bp+0, -0x1.e2d5f1238d4c0p-56),
                quad(0x1.670d100d10754p+2, 0x1.fa696398ba718p-52)),
            std::make_tuple(quad(0x1.5832e2e5d966bp+5, 0x1.8794e6d1f4d78p-49),
                quad(0x1.0d0ced69dd7b7p+62, -0x1.281cd9a4320d3p+8)),
            std::make_tuple(quad(0x1.15d8c5d3e4004p+9, -0x1.01f96f5f98c28p-45),
                quad(0x1.9ecf9ee00e06bp+801, 0x1.8ebd36d627d80p+747)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = expm1(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(0.0);

        const quad actual = expm1(input);

        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::log") {
    using num_collect::multi_double::log;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.01c016cf3b315p-29, -0x1.ec067815a7138p-84),
                quad(-0x1.4182e26a42f3ep+4, 0x1.ddb996c01104ap-50)),
            std::make_tuple(quad(0x1.197e3eb11c9f1p-11, 0x1.600b1b3bfbf70p-65),
                quad(-0x1.e1e669ea72951p+2, 0x1.6fa409eea7fadp-52)),
            std::make_tuple(quad(0x1.1d050b4ff7bbfp-2, -0x1.5cd3ea0636098p-57),
                quad(-0x1.4766dfe3cc8f7p+0, -0x1.684aba195d050p-55)),
            std::make_tuple(quad(0x1.f638b3e0e9e53p-1, -0x1.39d9b27ddcc18p-55),
                quad(-0x1.3bf059929289ap-6, 0x1.854e79ada7d8dp-61)),
            std::make_tuple(quad(0x1.5957a572db657p+1, -0x1.ba4f2cdf77ab8p-53),
                quad(0x1.fc29c030db576p-1, -0x1.064e0ce82f619p-56)),
            std::make_tuple(quad(0x1.ec8b6be14f39fp+19, 0x1.780a9e47ae320p-35),
                quad(0x1.ba5fe121ec9e6p+3, 0x1.c34ba2da67812p-53)),
            std::make_tuple(quad(0x1.ab2d7adaa1156p+33, -0x1.a5b71b73af190p-23),
                quad(0x1.762c8ffa9ef99p+4, -0x1.288867257e4c6p-51)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = log(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for one") {
        const quad input(1.0);
        const quad expected(0.0);

        const quad actual = log(input);

        CHECK(actual == expected);
    }

    SECTION("calculate for zero") {
        const quad input(0.0);

        const quad actual = log(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(actual < 0.0);
    }

    SECTION("calculate for a negative value") {
        const quad input(-1.0);

        const quad actual = log(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(std::isnan(actual.high()));
    }
}

TEST_CASE("num_collect::multi_double::log1p") {
    using num_collect::multi_double::log1p;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(
                quad(-0x1.0502c4a06925ep-10, -0x1.958f20a5e1030p-64),
                quad(-0x1.05240e1d32d3cp-10, -0x1.ba17fbb28ed42p-64)),
            std::make_tuple(
                quad(-0x1.eb9d5a6f5e582p-13, -0x1.3ce4a88bcd750p-69),
                quad(-0x1.ebac1b5a60ad6p-13, 0x1.de3158dfb888cp-68)),
            std::make_tuple(quad(0x1.800465b45ad46p-48, 0x1.a273de5b32fd8p-103),
                quad(0x1.800465b45ad34p-48, 0x1.9f2792419199cp-103)),
            std::make_tuple(quad(0x1.11e0a04f502acp-9, 0x1.f80d9d284506cp-63),
                quad(0x1.11977a2d596d3p-9, -0x1.1e926d1e549ecp-63)),
            std::make_tuple(quad(0x1.067ec9978934cp+15, -0x1.c28e47aebe040p-39),
                quad(0x1.4d836cbe7d2dfp+3, 0x1.ab32b8c12b0d7p-51)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = log1p(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::log10") {
    using num_collect::multi_double::log10;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.01c016cf3b315p-29, -0x1.ec067815a7138p-84),
                quad(-0x1.1742d9f9958c5p+3, 0x1.94691d3552870p-52)),
            std::make_tuple(quad(0x1.197e3eb11c9f1p-11, 0x1.600b1b3bfbf70p-65),
                quad(-0x1.a292b499e8026p+1, 0x1.c6edc9d96eeb0p-53)),
            std::make_tuple(quad(0x1.1d050b4ff7bbfp-2, -0x1.5cd3ea0636098p-57),
                quad(-0x1.1c60acd057bafp-1, -0x1.f9edcedb44f01p-57)),
            std::make_tuple(quad(0x1.5957a572db657p+1, -0x1.ba4f2cdf77ab8p-53),
                quad(0x1.b9628594ceca1p-2, 0x1.484739e5283ffp-56)),
            std::make_tuple(quad(0x1.ec8b6be14f39fp+19, 0x1.780a9e47ae320p-35),
                quad(0x1.803ddbb5c0d13p+2, 0x1.7d25276139c81p-55)),
            std::make_tuple(quad(0x1.ab2d7adaa1156p+33, -0x1.a5b71b73af190p-23),
                quad(0x1.4500e35f71782p+3, -0x1.ee544ea574060p-53)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = log10(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for one") {
        const quad input(1.0);
        const quad expected(0.0);

        const quad actual = log10(input);

        CHECK(actual == expected);
    }

    SECTION("calculate for zero") {
        const quad input(0.0);

        const quad actual = log10(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(actual < 0.0);
    }

    SECTION("calculate for a negative value") {
        const quad input(-1.0);

        const quad actual = log10(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(std::isnan(actual.high()));
    }
}

TEST_CASE("num_collect::multi_double::pow(quad, quad)") {
    using num_collect::multi_double::pow;
    using num_collect::multi_double::quad;

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
            std::make_tuple(quad(0x1.6cda60be6b14fp-2, 0x1.399372f983e40p-56),
                quad(0x1.0000000000000p+1, 0x0.0p+0),
                quad(0x1.03fedeb334b0bp-3, -0x1.3da45e9c7b7a4p-57)),
            std::make_tuple(quad(0x1.2c05b961881ddp+9, 0x1.53c917c681b20p-45),
                quad(0x1.9000000000000p+6, 0x0.0p+0),
                quad(0x1.db46a2ddff7bcp+922, -0x1.e96b9fff8c450p+866)),
            std::make_tuple(quad(0x1.d1b18f6ad52ffp-2, -0x1.4ec7fb6021200p-62),
                quad(-0x1.2c00000000000p+8, 0x0.0p+0),
                quad(0x1.052adfbd40811p+341, -0x1.87e957bcb8680p+284)),
            std::make_tuple(quad(0x1.dad80bdd56581p-10, -0x1.f97511af07900p-70),
                quad(0x1.0000000000000p-1, 0x0.0p+0),
                quad(0x1.5ca795647210ap-5, -0x1.a71cf5472d030p-60)),
            std::make_tuple(quad(0x1.645eaa2f888ecp-7, 0x1.11285a34d2f78p-62),
                quad(0x0.0p+0, 0x0.0p+0), quad(0x1.0000000000000p+0, 0x0.0p+0)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("base: " << format_quad_for_test(base));
        INFO("exponent: " << format_quad_for_test(exponent));

        const quad actual = pow(base, exponent);
        constexpr quad relative_tolerance(0x1.0p-90);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::pow(quad, double)") {
    using num_collect::multi_double::pow;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad base;
        double exponent{};
        quad expected;
        std::tie(base, exponent,
            expected) = GENERATE(Catch::Generators::table<quad, double, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.be580cb8d6672p+5, -0x1.7c1b0862537e4p-49),
                0x1.7ff89514e4ba3p+2,
                quad(0x1.c0a64f3223e63p+34, -0x1.045ce8d712c3cp-20)),
            std::make_tuple(quad(0x1.5a2b98fe9585cp-10, -0x1.0c36fa4e937a0p-66),
                0x1.377eb204dfb45p+2,
                quad(0x1.5d29dca66ad78p-47, -0x1.c1618a26a885cp-101)),
            std::make_tuple(quad(0x1.abf6487b5305fp+5, 0x1.924c526aeb870p-50),
                0x1.5b7d6d2e1b674p-2,
                quad(0x1.edf9f83e3942fp+1, -0x1.482073d138208p-53)),
            std::make_tuple(quad(0x1.5df1c02a3661fp-12, 0x1.98c0818650188p-67),
                -0x1.30a1b401d3ba5p+3,
                quad(0x1.ec627b697f060p+109, -0x1.cd7c251297920p+54)),
            std::make_tuple(quad(0x1.17ce6ac42961cp-8, -0x1.070ccd508d778p-63),
                -0x1.1f55dcd10d285p-3,
                quad(0x1.13368fce32b95p+1, 0x1.66ee5a266fc08p-54)),
            std::make_tuple(quad(0x1.6cda60be6b14fp-2, 0x1.399372f983e40p-56),
                0x1.0000000000000p+1,
                quad(0x1.03fedeb334b0bp-3, -0x1.3da45e9c7b7a4p-57)),
            std::make_tuple(quad(0x1.2c05b961881ddp+9, 0x1.53c917c681b20p-45),
                0x1.9000000000000p+6,
                quad(0x1.db46a2ddff7bcp+922, -0x1.e96b9fff8c450p+866)),
            std::make_tuple(quad(0x1.d1b18f6ad52ffp-2, -0x1.4ec7fb6021200p-62),
                -0x1.2c00000000000p+8,
                quad(0x1.052adfbd40811p+341, -0x1.87e957bcb8680p+284)),
            std::make_tuple(quad(0x1.dad80bdd56581p-10, -0x1.f97511af07900p-70),
                0x1.0000000000000p-1,
                quad(0x1.5ca795647210ap-5, -0x1.a71cf5472d030p-60)),
            std::make_tuple(quad(0x1.645eaa2f888ecp-7, 0x1.11285a34d2f78p-62),
                0x0.0p+0, quad(0x1.0000000000000p+0, 0x0.0p+0)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("base: " << format_quad_for_test(base));
        INFO("exponent: " << format_quad_for_test(exponent));

        const quad actual = pow(base, exponent);
        constexpr quad relative_tolerance(0x1.0p-90);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::pow(quad, unsigned int)") {
    using num_collect::multi_double::pow;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad base;
        unsigned int exponent{};
        quad expected;
        std::tie(
            base, exponent, expected) = GENERATE(Catch::Generators::table<quad,
            unsigned int, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.7e55eca788245p+5, 0x1.d2c49ebca1298p-50),
                0U, quad(0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.be580cb8d6672p+5, -0x1.7c1b0862537e4p-49),
                1U, quad(0x1.be580cb8d6672p+5, -0x1.7c1b0862537e4p-49)),
            std::make_tuple(quad(0x1.5a2b98fe9585cp-10, -0x1.0c36fa4e937a0p-66),
                3U, quad(0x1.3c7d5917c1d42p-29, 0x1.4c0fc0ed47200p-86)),
            std::make_tuple(quad(0x1.abf6487b5305fp+5, 0x1.924c526aeb870p-50),
                10U, quad(0x1.54f1a0875d249p+57, 0x1.5ebf6de7c21d8p+2)),
            std::make_tuple(quad(0x1.5df1c02a3661fp-1, 0x1.98c0818650188p-57),
                100U, quad(0x1.12067fc5ef49ep-55, -0x1.6a922fa1c14a0p-112)),
            std::make_tuple(quad(0x1.17ce6ac42961cp+0, -0x1.070ccd508d778p-55),
                1024U, quad(0x1.495e1a4023cf3p+131, 0x1.b42de2ff34708p+77)),
            std::make_tuple(quad(0x1.144a41d817304p+0, 0x1.f971a86a04718p-54),
                1025U, quad(0x1.baea196756e9fp+112, -0x1.fafbbbaa69d30p+56)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("base: " << format_quad_for_test(base));
        INFO("exponent: " << exponent);

        const quad actual = pow(base, exponent);
        constexpr quad relative_tolerance(0x1.0p-90);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::pow(quad, int)") {
    using num_collect::multi_double::pow;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad base;
        int exponent{};
        quad expected;
        std::tie(base, exponent,
            expected) = GENERATE(Catch::Generators::table<quad, int, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.0c05b74da84a2p+0, -0x1.e8d06162c6b5cp-54),
                -1025, quad(0x1.192fb5a6ea8bep-68, -0x1.5a14deaaa6eacp-122)),
            std::make_tuple(quad(0x1.a9d0751a1b25ap-1, -0x1.7a911ad8d16f4p-55),
                -1024, quad(0x1.3b816b00f0323p+272, 0x1.b78835d59f000p+210)),
            std::make_tuple(quad(0x1.582f99cdb6be9p-3, -0x1.f16591b5a67acp-57),
                -90, quad(0x1.7afce57d2e86ep+231, 0x1.073462a94c968p+176)),
            std::make_tuple(quad(0x1.db8b052be4d4fp+24, -0x1.cfb2d37c6e56cp-30),
                -4, quad(0x1.580068d68b84dp-100, 0x1.d766b8584fcdcp-154)),
            std::make_tuple(quad(0x1.7aa3801ec43e4p-8, -0x1.f923144bdc7f4p-62),
                -1, quad(0x1.5a2a9b836fe19p+7, 0x1.75ac58a7918f0p-49)),
            std::make_tuple(quad(0x1.7e55eca788245p+5, 0x1.d2c49ebca1298p-50),
                0, quad(0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.be580cb8d6672p+5, -0x1.7c1b0862537e4p-49),
                1, quad(0x1.be580cb8d6672p+5, -0x1.7c1b0862537e4p-49)),
            std::make_tuple(quad(0x1.5a2b98fe9585cp-10, -0x1.0c36fa4e937a0p-66),
                3, quad(0x1.3c7d5917c1d42p-29, 0x1.4c0fc0ed47200p-86)),
            std::make_tuple(quad(0x1.abf6487b5305fp+5, 0x1.924c526aeb870p-50),
                10, quad(0x1.54f1a0875d249p+57, 0x1.5ebf6de7c21d8p+2)),
            std::make_tuple(quad(0x1.5df1c02a3661fp-1, 0x1.98c0818650188p-57),
                100, quad(0x1.12067fc5ef49ep-55, -0x1.6a922fa1c14a0p-112)),
            std::make_tuple(quad(0x1.17ce6ac42961cp+0, -0x1.070ccd508d778p-55),
                1024, quad(0x1.495e1a4023cf3p+131, 0x1.b42de2ff34708p+77)),
            std::make_tuple(quad(0x1.144a41d817304p+0, 0x1.f971a86a04718p-54),
                1025, quad(0x1.baea196756e9fp+112, -0x1.fafbbbaa69d30p+56)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("base: " << format_quad_for_test(base));
        INFO("exponent: " << exponent);

        const quad actual = pow(base, exponent);
        constexpr quad relative_tolerance(0x1.0p-90);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::sin") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::sin;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.921fb54442d18p-2, 0x1.1a62633145c08p-56),
                quad(0x1.87de2a6aea963p-2, -0x1.72cedd3d5a60ep-57)),
            std::make_tuple(quad(0x1.2d97c7f3321d2p+0, 0x1.a79394c9e8a10p-55),
                quad(0x1.d906bcf328d46p-1, 0x1.457e610231ac6p-56)),
            std::make_tuple(quad(0x1.f6a7a2955385ep+0, 0x1.60fafbfd97308p-54),
                quad(0x1.d906bcf328d46p-1, 0x1.457e610231ac3p-56)),
            std::make_tuple(quad(0x1.5fdbbe9bba775p+1, 0x1.ee2c2d963a110p-54),
                quad(0x1.87de2a6aea963p-2, -0x1.72cedd3d5a62dp-57)),
            std::make_tuple(quad(0x1.c463abeccb2bbp+1, 0x1.3daeaf976e788p-53),
                quad(-0x1.87de2a6aea963p-2, 0x1.72cedd3d5a60cp-57)),
            std::make_tuple(quad(0x1.1475cc9eedf01p+2, -0x1.3ddc5bce200bcp-52),
                quad(-0x1.d906bcf328d46p-1, -0x1.457e610231abdp-56)),
            std::make_tuple(quad(0x1.46b9c347764a4p+2, -0x1.1a900f67f7538p-52),
                quad(-0x1.d906bcf328d46p-1, -0x1.457e610231ab3p-56)),
            std::make_tuple(quad(0x1.78fdb9effea47p+2, -0x1.ee8786039d370p-53),
                quad(-0x1.87de2a6aea963p-2, 0x1.72cedd3d5a63ep-57)),
            std::make_tuple(quad(0x1.ab41b09886feap+2, -0x1.a7eeed374bc70p-53),
                quad(0x1.87de2a6aea963p-2, -0x1.72cedd3d5a5fcp-57)),
            std::make_tuple(quad(0x1.284ed19fda3c1p-6, -0x1.323876d6af6e4p-60),
                quad(0x1.284aaf1314df0p-6, 0x1.198254ed96bfep-60)),
            std::make_tuple(quad(0x1.807eb74675238p+4, 0x1.10f7197fc8ba4p-50),
                quad(-0x1.c8b770408dc53p-1, -0x1.4d118e6f5dd9bp-56)),
            std::make_tuple(quad(0x1.7861bf3a3a90fp-6, -0x1.1a15c2c1ded18p-61),
                quad(0x1.785945b59ad25p-6, 0x1.27ea58f56d7bap-60)),
            std::make_tuple(quad(-0x1.d63e4eda10237p-9, -0x1.3debc994dbed8p-64),
                quad(-0x1.d63e0cbd887ccp-9, -0x1.49d3f930dc66ep-63)),
            std::make_tuple(quad(-0x1.6b4fee68b4baap+3, 0x1.f3bba23239db0p-51),
                quad(0x1.df8d0c82494e5p-1, -0x1.aacae5aca431bp-56)),
            std::make_tuple(
                quad(-0x1.e76a585102824p-28, -0x1.3e79f43e85a60p-82),
                quad(-0x1.e76a585102824p-28, -0x1.e9b5884a27d49p-83)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = sin(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::cos") {
    using num_collect::multi_double::cos;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.921fb54442d18p-2, 0x1.1a62633145c08p-56),
                quad(0x1.d906bcf328d46p-1, 0x1.457e610231ac1p-56)),
            std::make_tuple(quad(0x1.2d97c7f3321d2p+0, 0x1.a79394c9e8a10p-55),
                quad(0x1.87de2a6aea963p-2, -0x1.72cedd3d5a625p-57)),
            std::make_tuple(quad(0x1.f6a7a2955385ep+0, 0x1.60fafbfd97308p-54),
                quad(-0x1.87de2a6aea963p-2, 0x1.72cedd3d5a615p-57)),
            std::make_tuple(quad(0x1.5fdbbe9bba775p+1, 0x1.ee2c2d963a110p-54),
                quad(-0x1.d906bcf328d46p-1, -0x1.457e610231ac8p-56)),
            std::make_tuple(quad(0x1.c463abeccb2bbp+1, 0x1.3daeaf976e788p-53),
                quad(-0x1.d906bcf328d46p-1, -0x1.457e610231ac1p-56)),
            std::make_tuple(quad(0x1.1475cc9eedf01p+2, -0x1.3ddc5bce200bcp-52),
                quad(-0x1.87de2a6aea963p-2, 0x1.72cedd3d5a5fbp-57)),
            std::make_tuple(quad(0x1.46b9c347764a4p+2, -0x1.1a900f67f7538p-52),
                quad(0x1.87de2a6aea963p-2, -0x1.72cedd3d5a5c9p-57)),
            std::make_tuple(quad(0x1.78fdb9effea47p+2, -0x1.ee8786039d370p-53),
                quad(0x1.d906bcf328d46p-1, 0x1.457e610231acbp-56)),
            std::make_tuple(quad(0x1.ab41b09886feap+2, -0x1.a7eeed374bc70p-53),
                quad(0x1.d906bcf328d46p-1, 0x1.457e610231abep-56)),
            std::make_tuple(quad(0x1.284ed19fda3c1p-6, -0x1.323876d6af6e4p-60),
                quad(0x1.ffea90c079720p-1, 0x1.223206a762eaep-57)),
            std::make_tuple(quad(0x1.807eb74675238p+4, 0x1.10f7197fc8ba4p-50),
                quad(0x1.ced5ae1141032p-2, 0x1.385f0dcacb32ap-56)),
            std::make_tuple(quad(0x1.7861bf3a3a90fp-6, -0x1.1a15c2c1ded18p-61),
                quad(0x1.ffdd6a6f3d1a8p-1, -0x1.c6c306eb18fd6p-55)),
            std::make_tuple(quad(-0x1.d63e4eda10237p-9, -0x1.3debc994dbed8p-64),
                quad(0x1.ffff280dd9015p-1, -0x1.40e5050410656p-62)),
            std::make_tuple(quad(-0x1.6b4fee68b4baap+3, 0x1.f3bba23239db0p-51),
                quad(0x1.66bf40022bf4bp-2, 0x1.e5ffa5f7079e4p-58)),
            std::make_tuple(
                quad(-0x1.e76a585102824p-28, -0x1.3e79f43e85a60p-82),
                quad(0x1.0000000000000p+0, -0x1.d002e418bc8ccp-56)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = cos(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::tan") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::tan;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.921fb54442d18p-2, 0x1.1a62633145c08p-56),
                quad(0x1.a827999fcef32p-2, 0x1.08b2fb1366eabp-56)),
            std::make_tuple(quad(0x1.2d97c7f3321d2p+0, 0x1.a79394c9e8a10p-55),
                quad(0x1.3504f333f9de6p+1, 0x1.21165f626cddfp-53)),
            std::make_tuple(quad(0x1.f6a7a2955385ep+0, 0x1.60fafbfd97308p-54),
                quad(-0x1.3504f333f9de6p+1, -0x1.21165f626cdd7p-53)),
            std::make_tuple(quad(0x1.5fdbbe9bba775p+1, 0x1.ee2c2d963a110p-54),
                quad(-0x1.a827999fcef32p-2, -0x1.08b2fb1366e97p-56)),
            std::make_tuple(quad(0x1.c463abeccb2bbp+1, 0x1.3daeaf976e788p-53),
                quad(0x1.a827999fcef32p-2, 0x1.08b2fb1366eacp-56)),
            std::make_tuple(quad(0x1.1475cc9eedf01p+2, -0x1.3ddc5bce200bcp-52),
                quad(0x1.3504f333f9de6p+1, 0x1.21165f626cdcbp-53)),
            std::make_tuple(quad(0x1.46b9c347764a4p+2, -0x1.1a900f67f7538p-52),
                quad(-0x1.3504f333f9de6p+1, -0x1.21165f626cdb4p-53)),
            std::make_tuple(quad(0x1.78fdb9effea47p+2, -0x1.ee8786039d370p-53),
                quad(-0x1.a827999fcef32p-2, -0x1.08b2fb1366e8cp-56)),
            std::make_tuple(quad(0x1.ab41b09886feap+2, -0x1.a7eeed374bc70p-53),
                quad(0x1.a827999fcef32p-2, 0x1.08b2fb1366eb6p-56)),
            std::make_tuple(quad(0x1.284ed19fda3c1p-6, -0x1.323876d6af6e4p-60),
                quad(0x1.285717092cd63p-6, -0x1.dc41aabf96583p-60)),
            std::make_tuple(quad(0x1.807eb74675238p+4, 0x1.10f7197fc8ba4p-50),
                quad(-0x1.f93b61d60c93cp+0, -0x1.9280e20318be5p-55)),
            std::make_tuple(quad(0x1.7861bf3a3a90fp-6, -0x1.1a15c2c1ded18p-61),
                quad(0x1.7872b34b538efp-6, 0x1.589f08d75917ep-61)),
            std::make_tuple(quad(-0x1.d63e4eda10237p-9, -0x1.3debc994dbed8p-64),
                quad(-0x1.d63ed31351a1cp-9, 0x1.5aef343246623p-63)),
            std::make_tuple(quad(-0x1.6b4fee68b4baap+3, 0x1.f3bba23239db0p-51),
                quad(0x1.56347c45c060ep+1, -0x1.1b5b35bfafbf1p-53)),
            std::make_tuple(
                quad(-0x1.e76a585102824p-28, -0x1.3e79f43e85a60p-82),
                quad(-0x1.e76a585102824p-28, -0x1.d1b85471691d7p-82)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = tan(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::asin") {
    using num_collect::multi_double::asin;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.0000000000000p+0, 0x0.0p+0),
                quad(-0x1.921fb54442d18p+0, -0x1.1a62633145c07p-54)),
            std::make_tuple(quad(-0x1.f858f4b0b2a03p-1, 0x1.c08ae3c4a76d4p-55),
                quad(-0x1.65ceb54ed5013p+0, 0x1.b6ebd73674b5dp-55)),
            std::make_tuple(quad(-0x1.6e29015574577p-5, -0x1.42a63fd385670p-60),
                quad(-0x1.6e483ecee0060p-5, -0x1.a359ced9faebfp-59)),
            std::make_tuple(quad(0x1.a8c218be4484dp-32, 0x1.a231ce8c92588p-87),
                quad(0x1.a8c218be4484dp-32, 0x1.a29340baf8262p-87)),
            std::make_tuple(quad(0x1.65528728f7924p-8, 0x1.69b0488d1a5e0p-63),
                quad(0x1.6552fb2f9e86cp-8, 0x1.686ab6978ab06p-62)),
            std::make_tuple(quad(0x1.9f63608ae2177p-1, -0x1.72f68eb27d260p-58),
                quad(0x1.e48c0c6020b2bp-1, 0x1.5f2eddab20b8cp-55)),
            std::make_tuple(quad(0x1.0000000000000p+0, 0x0.0p+0),
                quad(0x1.921fb54442d18p+0, 0x1.1a62633145c07p-54)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = asin(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(0.0);

        const quad actual = asin(input);
        CHECK(actual == expected);
    }

    SECTION("calculate for large values") {
        const quad input(1.0, 1e-30);

        const quad actual = asin(input);
        CHECK(std::isnan(actual.high()));
    }

    SECTION("calculate for small values") {
        const quad input(-1.0, -1e-30);

        const quad actual = asin(input);
        CHECK(std::isnan(actual.high()));
    }
}

TEST_CASE("num_collect::multi_double::acos") {
    using num_collect::multi_double::acos;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.0000000000000p+0, 0x0.0p+0),
                quad(0x1.921fb54442d18p+1, 0x1.1a62633145c07p-53)),
            std::make_tuple(quad(-0x1.f858f4b0b2a03p-1, 0x1.c08ae3c4a76d4p-55),
                quad(0x1.7bf735498be96p+1, -0x1.e089c434fa4d4p-53)),
            std::make_tuple(quad(-0x1.6e29015574577p-5, -0x1.42a63fd385670p-60),
                quad(0x1.9d91f73ab9d1bp+0, 0x1.277d31a81597dp-54)),
            std::make_tuple(quad(0x1.a8c218be4484dp-32, 0x1.a231ce8c92588p-87),
                quad(0x1.921fb5429a0f7p+0, -0x1.152ebe038b893p-54)),
            std::make_tuple(quad(0x1.65528728f7924p-8, 0x1.69b0488d1a5e0p-63),
                quad(0x1.90ba624913330p+0, -0x1.2e0c0f0aa3948p-55)),
            std::make_tuple(quad(0x1.9f63608ae2177p-1, -0x1.72f68eb27d260p-58),
                quad(0x1.3fb35e2864f05p-1, 0x1.ab2bd16ed5904p-56)),
            std::make_tuple(
                quad(0x1.0000000000000p+0, 0x0.0p+0), quad(0x0.0p+0, 0x0.0p+0)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = acos(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected = num_collect::multi_double::impl::pi_over_2_quad;

        const quad actual = acos(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for large values") {
        const quad input(1.0, 1e-30);

        const quad actual = asin(input);
        CHECK(std::isnan(actual.high()));
    }

    SECTION("calculate for small values") {
        const quad input(-1.0, -1e-30);

        const quad actual = asin(input);
        CHECK(std::isnan(actual.high()));
    }
}

TEST_CASE("num_collect::multi_double::atan") {
    using num_collect::multi_double::atan;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.81b6443eaab93p+35, 0x1.a72f57922eef8p-20),
                quad(-0x1.921fb5442d947p+0, 0x1.bb23cfee81d5dp-55)),
            std::make_tuple(quad(-0x1.669ed2612fc1cp+7, -0x1.ff9fcb47b9e00p-53),
                quad(-0x1.90b238ead9f1ep+0, 0x1.d19d4360964c1p-54)),
            std::make_tuple(quad(-0x1.0000000000000p+0, 0x0.0p+0),
                quad(-0x1.921fb54442d18p-1, -0x1.1a62633145c07p-55)),
            std::make_tuple(quad(-0x1.5bc7759f2479ep-2, 0x1.370ac47213f70p-57),
                quad(-0x1.4f43469bde1ffp-2, -0x1.bcddcdba17c4dp-56)),
            std::make_tuple(quad(-0x1.c792b1acb54c8p-3, -0x1.de97dcdf1ce84p-57),
                quad(-0x1.c0462f0ee4d55p-3, -0x1.d834a222f8519p-62)),
            std::make_tuple(quad(-0x1.d3f23b88b6ab2p-41, 0x1.2e25012d428a0p-96),
                quad(-0x1.d3f23b88b6ab2p-41, 0x1.2e25016e6841dp-96)),
            std::make_tuple(quad(0x1.592550521dba1p-42, 0x1.b8ec35b72f7e8p-96),
                quad(0x1.592550521dba1p-42, 0x1.b8ec35b3eafe4p-96)),
            std::make_tuple(quad(0x1.076d4d74eca62p-1, -0x1.5065cd615be8cp-55),
                quad(0x1.e696d6e427a0fp-2, 0x1.2b9d82418a8c3p-56)),
            std::make_tuple(quad(0x1.0000000000000p+0, 0x0.0p+0),
                quad(0x1.921fb54442d18p-1, 0x1.1a62633145c07p-55)),
            std::make_tuple(quad(0x1.5eda751c2c50fp+1, -0x1.5d54e671ea9d0p-53),
                quad(0x1.3891e39b73475p+0, 0x1.177a04405f124p-57)),
            std::make_tuple(quad(0x1.65c5033df841fp+19, -0x1.1c8698d601558p-36),
                quad(0x1.921f9e5e8590ep+0, 0x1.87596b527b805p-55)),
            std::make_tuple(quad(0x1.94ae79531ccc5p+42, 0x1.3b24e86e7e6d8p-13),
                quad(0x1.921fb54442a90p+0, 0x1.fd33632baddc4p-54)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = atan(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for infinity") {
        const quad input = std::numeric_limits<double>::infinity();
        const quad expected = num_collect::multi_double::impl::pi_over_2_quad;

        const quad actual = atan(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for -infinity") {
        const quad input = -std::numeric_limits<double>::infinity();
        const quad expected = -num_collect::multi_double::impl::pi_over_2_quad;

        const quad actual = atan(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::atan2") {
    using num_collect::multi_double::atan2;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad x;
        quad y;
        quad expected;
        std::tie(x, y,
            expected) = GENERATE(Catch::Generators::table<quad, quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.6bc343cde7d78p+4, -0x1.6ec624ec8377cp-50),
                quad(0x1.bdc89c1b355f6p+5, 0x1.4b1291c09af00p-53),
                quad(0x1.2ef3e962ba84bp+0, -0x1.3ca0c36a6da60p-56)),
            std::make_tuple(quad(0x1.3b807cf424316p+5, 0x1.dbef1155dcc00p-53),
                quad(0x1.31de8194c625ep-3, -0x1.e355f96cf3440p-57),
                quad(0x1.f05d91d70ccb5p-9, -0x1.c407ddcdedaccp-63)),
            std::make_tuple(quad(-0x1.ef79cb69682b3p+4, -0x1.baf6037aca920p-50),
                quad(0x1.2e413fa320922p-26, 0x1.bc40a8e9b8dc0p-80),
                quad(0x1.921fb5430a7bbp+1, 0x1.f631194219d2cp-53)),
            std::make_tuple(quad(-0x1.f83d67f10f754p-9, -0x1.336e7c43305a0p-63),
                quad(0x1.41e44f511915ep+2, 0x1.7ec6660534cf0p-52),
                quad(0x1.9251d5f02b900p+0, 0x1.92996a9cdc598p-54)),
            std::make_tuple(quad(-0x1.caabfff431e59p-1, -0x1.45d69a833ede8p-55),
                quad(-0x1.ceffedf90709bp-39, 0x1.73d1b3e7f3b38p-93),
                quad(-0x1.921fb54440ccbp+1, 0x1.547c8ae9b5740p-57)),
            std::make_tuple(quad(-0x1.44c1e07279ba0p+8, 0x1.edf61195839d0p-46),
                quad(-0x1.cd4129453a9eep+23, -0x1.a2e5334833370p-32),
                quad(-0x1.92211dc0a6534p+0, 0x1.47210e57152e0p-55)),
            std::make_tuple(quad(0x1.67dcf604801f0p-24, 0x1.eb129515c9954p-78),
                quad(-0x1.613d97a2deb7ep-26, -0x1.782271675e068p-80),
                quad(-0x1.ecd6b53cfbd96p-3, -0x1.6cde7a81e2300p-58)),
            std::make_tuple(quad(0x1.89c0e175ffb91p+6, -0x1.fd08ee8c9f4acp-48),
                quad(-0x1.9d56a4d62f911p+19, -0x1.6553801ca5aa0p-36),
                quad(-0x1.9218164e57ef4p+0, -0x1.adc1a3e7d1e80p-59)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("x: " << format_quad_for_test(x));
        INFO("y: " << format_quad_for_test(y));

        const quad actual = atan2(y, x);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zeros") {
        const quad x(0.0);
        const quad y(0.0);

        const quad actual = atan2(y, x);
        CHECK(std::isnan(actual.high()));
    }
}

TEST_CASE("num_collect::multi_double::sinh") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::sinh;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.86b257fa21711p+8, -0x1.2a5a2e167baa8p-46),
                quad(-0x1.936aa4a814ad9p+562, -0x1.88a77b52b55e8p+506)),
            std::make_tuple(quad(-0x1.a96c8c1fe280ap-3, 0x1.1deefd798cca0p-59),
                quad(-0x1.ac7d7a96e192dp-3, 0x1.8bcee97468237p-57)),
            std::make_tuple(
                quad(-0x1.0a98bfb3fdaf8p-22, -0x1.6b4e354230ce8p-76),
                quad(-0x1.0a98bfb3fdb29p-22, 0x1.d4da3808ed83ep-76)),
            std::make_tuple(
                quad(-0x1.04824626c4105p-48, 0x1.1e4507ff1e4b8p-103),
                quad(-0x1.04824626c4105p-48, 0x1.1e4507ff1e350p-103)),
            std::make_tuple(
                quad(0x1.6e9219342dc4dp-49, -0x1.e01ec099a42d8p-103),
                quad(0x1.6e9219342dc4dp-49, -0x1.e01ec099a425bp-103)),
            std::make_tuple(quad(0x1.db931072e257dp-10, -0x1.cc1d6d3cdf740p-68),
                quad(0x1.db93218b8f962p-10, -0x1.2e8228a67e6bdp-66)),
            std::make_tuple(quad(0x1.0dd4be2b57836p-1, 0x1.1326afb4ab2e0p-58),
                quad(0x1.1a7f0bad8f04fp-1, -0x1.fcc816caa066fp-55)),
            std::make_tuple(quad(0x1.8ff7def2ecb82p+7, 0x1.5e17780a516e0p-47),
                quad(0x1.6e1a23e145ddap+287, -0x1.09b4e14c1fdcap+233)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = sinh(input);
        constexpr quad relative_tolerance(0x1.0p-98);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0x0.0p+0, 0x0.0p+0);
        const quad expected(0x0.0p+0, 0x0.0p+0);

        const quad actual = sinh(input);
        CHECK(actual == expected);
    }

    SECTION("calculate for too large value") {
        const auto input = quad(710.0);

        const quad actual = sinh(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(std::isinf(actual.high()));
        CHECK(actual.high() > 0.0);
    }

    SECTION("calculate for too small value") {
        const auto input = quad(-708.0);

        const auto actual = sinh(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(std::isinf(actual.high()));
        CHECK(actual.high() < 0.0);
    }
}

TEST_CASE("num_collect::multi_double::cosh") {
    using num_collect::multi_double::cosh;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.86b257fa21711p+8, -0x1.2a5a2e167baa8p-46),
                quad(0x1.936aa4a814ad9p+562, 0x1.88a77b52b55e8p+506)),
            std::make_tuple(quad(-0x1.a96c8c1fe280ap-3, 0x1.1deefd798cca0p-59),
                quad(0x1.058b0aa74581ap+0, -0x1.8d0f44cbcc4eep-54)),
            std::make_tuple(
                quad(-0x1.0a98bfb3fdaf8p-22, -0x1.6b4e354230ce8p-76),
                quad(0x1.000000000008bp+0, -0x1.78d9e6fe0964ep-55)),
            std::make_tuple(
                quad(-0x1.04824626c4105p-48, 0x1.1e4507ff1e4b8p-103),
                quad(0x1.0000000000000p+0, 0x1.0918e0c000000p-97)),
            std::make_tuple(
                quad(0x1.6e9219342dc4dp-49, -0x1.e01ec099a42d8p-103),
                quad(0x1.0000000000000p+0, 0x1.067309c000000p-98)),
            std::make_tuple(quad(0x1.db931072e257dp-10, -0x1.cc1d6d3cdf740p-68),
                quad(0x1.00001b9bd91b4p+0, -0x1.e50a63e2fc266p-55)),
            std::make_tuple(quad(0x1.0dd4be2b57836p-1, 0x1.1326afb4ab2e0p-58),
                quad(0x1.2461b48f85d91p+0, 0x1.46f74f27c9803p-55)),
            std::make_tuple(quad(0x1.8ff7def2ecb82p+7, 0x1.5e17780a516e0p-47),
                quad(0x1.6e1a23e145ddap+287, -0x1.09b4e14c1fdcap+233)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = cosh(input);
        constexpr quad relative_tolerance(0x1.0p-98);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0x0.0p+0, 0x0.0p+0);
        const quad expected(0x1.0p+0, 0x0.0p+0);

        const quad actual = cosh(input);
        CHECK(actual == expected);
    }

    SECTION("calculate for too large value") {
        const auto input = quad(710.0);

        const quad actual = cosh(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(std::isinf(actual.high()));
        CHECK(actual.high() > 0.0);
    }

    SECTION("calculate for too small value") {
        const auto input = quad(-708.0);

        const auto actual = cosh(input);

        INFO("actual: " << format_quad_for_test(actual));
        CHECK(std::isinf(actual.high()));
        CHECK(actual.high() > 0.0);
    }
}

TEST_CASE("num_collect::multi_double::tanh") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::tanh;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.86b257fa21711p+8, -0x1.2a5a2e167baa8p-46),
                quad(-0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.a96c8c1fe280ap-3, 0x1.1deefd798cca0p-59),
                quad(-0x1.a368a41d12df7p-3, -0x1.cc8939802a7b4p-58)),
            std::make_tuple(
                quad(-0x1.0a98bfb3fdaf8p-22, -0x1.6b4e354230ce8p-76),
                quad(-0x1.0a98bfb3fda98p-22, 0x1.460f02786ca90p-80)),
            std::make_tuple(
                quad(-0x1.04824626c4105p-48, 0x1.1e4507ff1e4b8p-103),
                quad(-0x1.04824626c4105p-48, 0x1.1e4507ff1e787p-103)),
            std::make_tuple(
                quad(0x1.6e9219342dc4dp-49, -0x1.e01ec099a42d8p-103),
                quad(0x1.6e9219342dc4dp-49, -0x1.e01ec099a43d3p-103)),
            std::make_tuple(quad(0x1.db931072e257dp-10, -0x1.cc1d6d3cdf740p-68),
                quad(0x1.db92ee418b2cdp-10, -0x1.b5948159b373dp-67)),
            std::make_tuple(quad(0x1.0dd4be2b57836p-1, 0x1.1326afb4ab2e0p-58),
                quad(0x1.eeb0789dcd574p-2, 0x1.0dea3642d7219p-57)),
            std::make_tuple(quad(0x1.8ff7def2ecb82p+7, 0x1.5e17780a516e0p-47),
                quad(0x1.0000000000000p+0, 0x0.0p+0)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = tanh(input);
        constexpr quad relative_tolerance(0x1.0p-96);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0x0.0p+0, 0x0.0p+0);
        const quad expected(0x0.0p+0, 0x0.0p+0);

        const quad actual = tanh(input);
        CHECK(actual == expected);
    }

    SECTION("calculate for too large value") {
        const auto input = quad(710.0);
        const quad expected(0x1.0p+0, 0x0.0p+0);

        const quad actual = tanh(input);
        CHECK(actual == expected);
    }

    SECTION("calculate for too small value") {
        const auto input = quad(-708.0);
        const quad expected(-0x1.0p+0, 0x0.0p+0);

        const auto actual = tanh(input);
        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::asinh") {
    using num_collect::multi_double::asinh;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(
                quad(-0x1.1ecf1dae30939p+39, -0x1.509a5c491b400p-20),
                quad(-0x1.bd6eb343ee218p+4, 0x1.3de21f2af69a4p-51)),
            std::make_tuple(quad(-0x1.13e2f13993d5ap+3, -0x1.183a1b317b3c0p-53),
                quad(-0x1.6ce54a2500a36p+1, -0x1.1a1efa9d01042p-53)),
            std::make_tuple(quad(-0x1.0759396169785p-11, 0x1.f5ecdb3ae8adcp-65),
                quad(-0x1.075938a79f533p-11, -0x1.b2995df178c2ap-65)),
            std::make_tuple(quad(-0x1.68e5691a87c5ep-32, 0x1.b27f0efafe080p-89),
                quad(-0x1.68e5691a87c5ep-32, 0x1.b36e23c76df9fp-89)),
            std::make_tuple(quad(0x1.44bcc8101fba9p-25, 0x1.3a48d94efba60p-82),
                quad(0x1.44bcc8101fba8p-25, -0x1.4a258921d8063p-79)),
            std::make_tuple(quad(0x1.dcd48122989b0p-9, -0x1.b596773838700p-68),
                quad(0x1.dcd43c34f7305p-9, 0x1.c921852cde45fp-63)),
            std::make_tuple(quad(0x1.d0327dcded31dp+10, -0x1.068f5953bba90p-44),
                quad(0x1.0708338dcecfep+3, 0x1.51a56d2e85bdep-51)),
            std::make_tuple(quad(0x1.9ef917859d81fp+46, 0x1.7e00e7b8a7110p-10),
                quad(0x1.087cd53526736p+5, 0x1.a3348cdac1274p-53)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = asinh(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0x0.0p+0, 0x0.0p+0);
        const quad expected(0x0.0p+0, 0x0.0p+0);

        const quad actual = asinh(input);
        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::acosh") {
    using num_collect::multi_double::acosh;
    using num_collect::multi_double::quad;

    SECTION("calculate for values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.02252dc716765p+0, -0x1.1991ea087ceb0p-55),
                quad(0x1.08f270e485dd3p-3, -0x1.f86a2dfe815c4p-58)),
            std::make_tuple(quad(0x1.d0327dcded31dp+10, -0x1.068f5953bba90p-44),
                quad(0x1.0708333ff2a23p+3, 0x1.0244c087069fbp-52)),
            std::make_tuple(quad(0x1.9ef917859d81fp+46, 0x1.7e00e7b8a7110p-10),
                quad(0x1.087cd53526736p+5, 0x1.a3348cdac0c5dp-53)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = acosh(input);
        constexpr quad relative_tolerance(0x1.0p-99);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for one") {
        const quad input(1.0);
        const quad expected(0.0);

        const quad actual = acosh(input);
        CHECK(actual == expected);
    }

    SECTION("calculate for less than one") {
        const quad input(1.0, -1e-30);

        const quad actual = acosh(input);

        CHECK(std::isnan(actual.high()));
    }
}

TEST_CASE("num_collect::multi_double::atanh") {
    using num_collect::multi_double::atanh;
    using num_collect::multi_double::quad;

    SECTION("calculate for values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.eecc475c72064p-1, 0x1.52e3fa5837064p-55),
                quad(-0x1.0472ba8d4abb5p+1, -0x1.821428c251f2bp-54)),
            std::make_tuple(quad(-0x1.63a7b3aa9fe62p-1, 0x1.6a76ccde58c50p-56),
                quad(-0x1.b6b6fd81e09abp-1, -0x1.32001cbac9aa6p-56)),
            std::make_tuple(quad(-0x1.19c4d7a962942p-7, -0x1.34c3b51ec3a60p-62),
                quad(-0x1.19c69ed0872dcp-7, 0x1.bd9f2f66cf857p-61)),
            std::make_tuple(quad(-0x1.d7b5b86e430bep-33, 0x1.c0648d22baa00p-91),
                quad(-0x1.d7b5b86e430bep-33, 0x1.be4eb22775be8p-91)),
            std::make_tuple(quad(0x1.c9e645feeaceep-48, 0x1.be9b281dbd348p-102),
                quad(0x1.c9e645feeaceep-48, 0x1.be9b281dbdae9p-102)),
            std::make_tuple(quad(0x1.6bfc419161f86p-26, -0x1.7e6aa79445bd0p-81),
                quad(0x1.6bfc419161f87p-26, -0x1.d43a89ce46373p-81)),
            std::make_tuple(quad(0x1.5635d7afaf2cep-1, 0x1.2e624f2d1fd50p-55),
                quad(0x1.9d9912bdf7712p-1, 0x1.f51893fd037d5p-55)),
            std::make_tuple(quad(0x1.b450f9a197e92p-1, 0x1.0a9fbc263c338p-55),
                quad(0x1.43999d0ec62dbp+0, -0x1.db7f7e3c85fadp-54)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = atanh(input);
        constexpr quad relative_tolerance(0x1.0p-98);
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(0.0);

        const quad actual = atanh(input);
        CHECK(actual == expected);
    }

    SECTION("calculate for 1") {
        const quad input(1.0);

        const quad actual = atanh(input);

        CHECK(std::isnan(actual.high()));
    }

    SECTION("calculate for -1") {
        const quad input(-1.0);

        const quad actual = atanh(input);

        CHECK(std::isnan(actual.high()));
    }
}

TEST_CASE("num_collect::multi_double::floor") {
    using num_collect::multi_double::floor;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58),
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58)),
            std::make_tuple(quad(-0x1.28f419c8f2c9dp+75, 0x1.42d614c040accp+21),
                quad(-0x1.28f419c8f2c9dp+75, 0x1.42d6100000000p+21)),
            std::make_tuple(
                quad(-0x1.112fc50d218b7p+35, -0x1.5625eaa60b200p-23),
                quad(-0x1.112fc50d40000p+35, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.1ac021183b471p-43, 0x1.ccdb580b5519cp-97),
                quad(-0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.762890cd17ad9p-15, -0x1.fa19057de5c00p-71),
                quad(0x0.0p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.1ad0d72802619p+8, -0x1.697d2b1918d60p-46),
                quad(0x1.1a00000000000p+8, 0x0.0p+0)),
            std::make_tuple(quad(0x1.05fbaf0942101p+70, 0x1.14570a7aa6020p+16),
                quad(0x1.05fbaf0942101p+70, 0x1.1457000000000p+16)),
            std::make_tuple(quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59),
                quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = floor(input);
        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::ceil") {
    using num_collect::multi_double::ceil;
    using num_collect::multi_double::quad;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58),
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58)),
            std::make_tuple(quad(-0x1.28f419c8f2c9dp+75, 0x1.42d614c040accp+21),
                quad(-0x1.28f419c8f2c9dp+75, 0x1.42d6180000000p+21)),
            std::make_tuple(
                quad(-0x1.112fc50d218b7p+35, -0x1.5625eaa60b200p-23),
                quad(-0x1.112fc50d20000p+35, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.1ac021183b471p-43, 0x1.ccdb580b5519cp-97),
                quad(0x0.0p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.762890cd17ad9p-15, -0x1.fa19057de5c00p-71),
                quad(0x1.0000000000000p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.1ad0d72802619p+8, -0x1.697d2b1918d60p-46),
                quad(0x1.1b00000000000p+8, 0x0.0p+0)),
            std::make_tuple(quad(0x1.05fbaf0942101p+70, 0x1.14570a7aa6020p+16),
                quad(0x1.05fbaf0942101p+70, 0x1.1458000000000p+16)),
            std::make_tuple(quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59),
                quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = ceil(input);
        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::trunc") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::trunc;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58),
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58)),
            std::make_tuple(quad(-0x1.28f419c8f2c9dp+75, 0x1.42d614c040accp+21),
                quad(-0x1.28f419c8f2c9dp+75, 0x1.42d6180000000p+21)),
            std::make_tuple(
                quad(-0x1.112fc50d218b7p+35, -0x1.5625eaa60b200p-23),
                quad(-0x1.112fc50d20000p+35, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.1ac021183b471p-43, 0x1.ccdb580b5519cp-97),
                quad(0x0.0p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.762890cd17ad9p-15, -0x1.fa19057de5c00p-71),
                quad(0x0.0p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.1ad0d72802619p+8, -0x1.697d2b1918d60p-46),
                quad(0x1.1a00000000000p+8, 0x0.0p+0)),
            std::make_tuple(quad(0x1.05fbaf0942101p+70, 0x1.14570a7aa6020p+16),
                quad(0x1.05fbaf0942101p+70, 0x1.1457000000000p+16)),
            std::make_tuple(quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59),
                quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = trunc(input);
        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::round") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::round;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58),
                quad(-0x1.1e7c7065eea5ep+112, 0x1.6dcd37ee57648p+58)),
            std::make_tuple(quad(-0x1.28f419c8f2c9dp+75, 0x1.42d614c040accp+21),
                quad(-0x1.28f419c8f2c9dp+75, 0x1.42d6180000000p+21)),
            std::make_tuple(
                quad(-0x1.112fc50d218b7p+35, -0x1.5625eaa60b200p-23),
                quad(-0x1.112fc50d20000p+35, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.4000000000000p+1, 0x0.0p+0),
                quad(-0x1.8000000000000p+1, 0x0.0p+0)),
            std::make_tuple(quad(-0x1.1ac021183b471p-43, 0x1.ccdb580b5519cp-97),
                quad(0x0.0p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.762890cd17ad9p-15, -0x1.fa19057de5c00p-71),
                quad(0x0.0p+0, 0x0.0p+0)),
            std::make_tuple(quad(0x1.8000000000000p+0, 0x0.0p+0),
                quad(0x1.0000000000000p+1, 0x0.0p+0)),
            std::make_tuple(quad(0x1.1ad0d72802619p+8, -0x1.697d2b1918d60p-46),
                quad(0x1.1b00000000000p+8, 0x0.0p+0)),
            std::make_tuple(quad(0x1.05fbaf0942101p+70, 0x1.14570a7aa6020p+16),
                quad(0x1.05fbaf0942101p+70, 0x1.1457000000000p+16)),
            std::make_tuple(quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59),
                quad(0x1.661bd7be881a1p+115, 0x1.15d90446d0dc0p+59)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = round(input);
        CHECK(actual == expected);
    }
}
