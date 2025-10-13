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
