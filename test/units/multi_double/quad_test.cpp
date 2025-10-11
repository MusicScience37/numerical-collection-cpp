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
 * \brief Test of quad class
 */
#include "num_collect/multi_double/quad.h"

#include <cstddef>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "format_quad_for_test.h"  // IWYU pragma: keep

TEST_CASE("num_collect::multi_double::quad") {
    using num_collect::multi_double::quad;

    SECTION("construct without arguments") {
        quad num;
        CHECK_THAT(num.high(), Catch::Matchers::WithinULP(0.0, 0));
        CHECK_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    SECTION("construct with double argument") {
        constexpr double value = 3.14;
        quad num = value;
        CHECK_THAT(num.high(), Catch::Matchers::WithinULP(value, 0));
        CHECK_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    SECTION("construct with int argument") {
        constexpr int value = 37;
        quad num = value;
        CHECK_THAT(num.high(),
            Catch::Matchers::WithinULP(static_cast<double>(value), 0));
        CHECK_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    constexpr std::uint64_t ulp_limit = 8;

    SECTION("add a quad number with operator+= (1)") {
        constexpr auto a = quad(0x1.0000000000001p+0, 0x0.7000000000001p-52);
        constexpr auto b = quad(0x1.0000000000007p+0, 0x0.8000000000002p-52);
        constexpr auto sum_true =
            quad(0x2.0000000000008p+0, 0x0.F000000000003p-52);
        auto sum = a;
        sum += b;
        CHECK_THAT(sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        CHECK_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a quad number with operator+= (2)") {
        constexpr auto a = quad(0x1.0000000000000p+0, 0x0.7p-52);
        constexpr auto b = quad(0x0.8000000000000p+0, 0x0.2p-52);
        constexpr auto sum_true = quad(0x1.8000000000001p+0, -0x0.7p-52);
        auto sum = a;
        sum += b;
        CHECK_THAT(sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        CHECK_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a quad number with operator+= (3)") {
        constexpr auto a = quad(0x1.0p+0, 0x0.7p-52);
        constexpr auto b = quad(-0x1.0p+0, 0x0.2p-52);
        constexpr auto sum_true = quad(0x0.9p-52);
        auto sum = a;
        sum += b;
        CHECK_THAT(sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        CHECK_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a quad number with operator+= (4)") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0x0.61E3F58931EC2p-52);
        // 0x3.1d8986dd9d1818a9a4ef90d801p+0
        constexpr auto sum_true =
            quad(0x3.1D8986DD9D182p+0, 0x0.8A9A4EF90D801p-52 - 0x1.0p-52);
        auto sum = a;
        sum += b;
        CHECK_THAT(sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        CHECK_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a quad number with operator+") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0x0.61E3F58931EC2p-52);
        // 0x3.1d8986dd9d1818a9a4ef90d801p+0
        constexpr auto sum_true =
            quad(0x3.1D8986DD9D182p+0, 0x0.8A9A4EF90D801p-52 - 0x1.0p-52);
        const auto sum = a + b;
        CHECK_THAT(sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        CHECK_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a quad number with operator+ (random)") {
        quad a;
        quad b;
        quad sum_true;
        std::tie(a, b,
            sum_true) = GENERATE(Catch::Generators::table<quad, quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.52b527b6c46a6p-17, 0x1.fb1f8716820c8p-71),
                quad(0x1.3735a527aa3a0p-8, 0x1.a73277b5f0aa0p-65),
                quad(0x1.37deffbb859c3p-8, 0x1.81e3deba49564p-62)),
            std::make_tuple(quad(0x1.15c15c22faf99p+7, 0x1.eaf4b47590204p-47),
                quad(-0x1.ae523e28a9262p-24, 0x1.2b1017f83acb8p-79),
                quad(0x1.15c15c1f9e552p+7, -0x1.295fde9944cfcp-47)),
            std::make_tuple(
                quad(-0x1.1246ca6607d7ep-47, -0x1.42dce60279808p-102),
                quad(0x1.323833e7757d9p+30, 0x1.5b3f99313c7a0p-26),
                quad(0x1.323833e7757d9p+30, 0x1.5b3f909f06270p-26)),
            std::make_tuple(quad(-0x1.cdfd31e1eeaafp+14, 0x1.6fa0de42b0780p-43),
                quad(-0x1.582805a6a9cfcp+5, 0x1.1c37331fcfdacp-49),
                quad(-0x1.cea945e4c1ffdp+14, -0x1.c97dc89e1a090p-40)),
            // cspell: enable
            // NOLINTEND
        }));

        const auto sum = a + b;
        CHECK_THAT(sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        CHECK_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a double number with operator+ (random)") {
        quad a;
        double b{};
        quad sum_true;
        std::tie(a, b,
            sum_true) = GENERATE(Catch::Generators::table<quad, double, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.58645c21bbf0ep-16, -0x1.647a8b7ec08c8p-71),
                0x1.c17cdbf807483p+16,
                quad(0x1.c17cdbf95fac9p+16, -0x1.ef220791647a8p-39)),
            std::make_tuple(quad(-0x1.7986766143f6ap+0, 0x1.19615589dc270p-56),
                0x1.4e79b4fedfba5p-31,
                quad(-0x1.7986765ea7033p+0, -0x1.3a37cd7588f64p-54)),
            std::make_tuple(quad(0x1.fd5d247a03357p-30, -0x1.09e10a59a3ca8p-85),
                -0x1.5a195eb27320ap+24,
                quad(-0x1.5a195eb27320ap+24, 0x1.fd5d247a03358p-30)),
            std::make_tuple(quad(0x1.57b12ae1c238dp-25, -0x1.d8caf920c9978p-80),
                0x1.1a52be8fe7d1ap+24,
                quad(0x1.1a52be8fe7d25p+24, -0x1.09daa3c7b8e68p-30)),
            // cspell: enable
            // NOLINTEND
        }));

        SECTION("quad + double") {
            const quad sum = a + b;
            CHECK_THAT(
                sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
            CHECK_THAT(sum.low(),
                Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
        }

        SECTION("double + quad") {
            const quad sum = b + a;
            CHECK_THAT(
                sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
            CHECK_THAT(sum.low(),
                Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
        }
    }

    SECTION("subtract a quad number with operator-= (1)") {
        constexpr auto a = quad(0x1.0000000000001p+0, 0x0.7000000000001p-52);
        constexpr auto b = quad(-0x1.0000000000007p+0, -0x0.8000000000002p-52);
        constexpr auto dif_true =
            quad(0x2.0000000000008p+0, 0x0.F000000000003p-52);
        auto dif = a;
        dif -= b;
        CHECK_THAT(dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        CHECK_THAT(
            dif.low(), Catch::Matchers::WithinULP(dif_true.low(), ulp_limit));
    }

    SECTION("subtract a quad number with operator-= (2)") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0x0.61E3F58931EC2p-52);
        // 0xd.8a26e30153e2c6d263e6a9a7dp-4
        constexpr auto dif_true =
            quad(0xD.8A26E30153E30p-4, 0xC.6D263E6A9A7Dp-56 - 0x10.0p-56);
        auto dif = a;
        dif -= b;
        CHECK_THAT(dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        CHECK_THAT(
            dif.low(), Catch::Matchers::WithinULP(dif_true.low(), ulp_limit));
    }

    SECTION("subtract a quad number with operator-") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0x0.61E3F58931EC2p-52);
        // 0xd.8a26e30153e2c6d263e6a9a7dp-4
        constexpr auto dif_true =
            quad(0xD.8A26E30153E30p-4, 0xC.6D263E6A9A7dp-56 - 0x10.0p-56);
        const auto dif = a - b;
        CHECK_THAT(dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        CHECK_THAT(
            dif.low(), Catch::Matchers::WithinULP(dif_true.low(), ulp_limit));
    }

    SECTION("subtract a quad number with operator- (random)") {
        quad a;
        quad b;
        quad dif_true;
        std::tie(a, b,
            dif_true) = GENERATE(Catch::Generators::table<quad, quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.52b527b6c46a6p-17, 0x1.fb1f8716820c8p-71),
                quad(0x1.3735a527aa3a0p-8, 0x1.a73277b5f0aa0p-65),
                quad(-0x1.368c4a93ced7dp-8, 0x1.181740cccd2bcp-62)),
            std::make_tuple(quad(0x1.15c15c22faf99p+7, 0x1.eaf4b47590204p-47),
                quad(-0x1.ae523e28a9262p-24, 0x1.2b1017f83acb8p-79),
                quad(0x1.15c15c26579e1p+7, 0x1.fe928f08ca208p-48)),
            std::make_tuple(
                quad(-0x1.1246ca6607d7ep-47, -0x1.42dce60279808p-102),
                quad(0x1.323833e7757d9p+30, 0x1.5b3f99313c7a0p-26),
                quad(-0x1.323833e7757d9p+30, -0x1.5b3fa1c372cd0p-26)),
            std::make_tuple(quad(-0x1.cdfd31e1eeaafp+14, 0x1.6fa0de42b0780p-43),
                quad(-0x1.582805a6a9cfcp+5, 0x1.1c37331fcfdacp-49),
                quad(-0x1.cd511ddf1b560p+14, -0x1.da99ffd139d90p-40)),
            // cspell: enable
            // NOLINTEND
        }));

        const auto dif = a - b;
        CHECK_THAT(dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        CHECK_THAT(
            dif.low(), Catch::Matchers::WithinULP(dif_true.low(), ulp_limit));
    }

    SECTION("subtract a double number with operator- (random)") {
        quad a;
        double b{};
        quad dif_true;
        std::tie(a, b,
            dif_true) = GENERATE(Catch::Generators::table<quad, double, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.58645c21bbf0ep-16, -0x1.647a8b7ec08c8p-71),
                0x1.c17cdbf807483p+16,
                quad(-0x1.c17cdbf6aee3dp+16, -0x1.ef220791647a8p-39)),
            std::make_tuple(quad(-0x1.7986766143f6ap+0, 0x1.19615589dc270p-56),
                0x1.4e79b4fedfba5p-31,
                quad(-0x1.79867663e0ea1p+0, 0x1.c6e8783a7709cp-54)),
            std::make_tuple(quad(0x1.fd5d247a03357p-30, -0x1.09e10a59a3ca8p-85),
                -0x1.5a195eb27320ap+24,
                quad(0x1.5a195eb27320ap+24, 0x1.fd5d247a03358p-30)),
            std::make_tuple(quad(0x1.57b12ae1c238dp-25, -0x1.d8caf920c9978p-80),
                0x1.1a52be8fe7d1ap+24,
                quad(-0x1.1a52be8fe7d0fp+24, -0x1.09daa3c7b8e68p-30)),
            // cspell: enable
            // NOLINTEND
        }));

        SECTION("quad - double") {
            const quad dif = a - b;
            CHECK_THAT(
                dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
            CHECK_THAT(dif.low(),
                Catch::Matchers::WithinULP(dif_true.low(), ulp_limit));
        }

        SECTION("double - quad") {
            const quad dif = -(b - a);
            CHECK_THAT(
                dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
            CHECK_THAT(dif.low(),
                Catch::Matchers::WithinULP(dif_true.low(), ulp_limit));
        }
    }

    SECTION("multiply quad numbers with operator*= (1)") {
        constexpr auto a = quad(0x1.0p+1, 0x0.1p-51);
        constexpr auto b = quad(0x1.0p+2, 0x0.2p-50);
        constexpr auto prod_true = quad(0x1.0p+3, 0x0.3p-49);
        auto prod = a;
        prod *= b;
        CHECK_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        CHECK_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION("multiply quad numbers with operator*= (2)") {
        constexpr auto a = quad(0x1.0000001p+1);
        constexpr auto b = quad(0x1.0000001p+2);
        constexpr auto prod_true = quad(0x1.0000002p+3, 0x0.1p-49);
        auto prod = a;
        prod *= b;
        CHECK_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        CHECK_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION("multiply quad numbers with operator*= (3)") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0x0.61E3F58931EC2p-52);
        // 0x2.3f53c6a82f11712dad9c5fd34db6p+0
        constexpr auto prod_true =
            quad(0x2.3F53C6A82F118p+0, 0x0.12DAD9C5FD34Dp-52 - 0x1.0p-52);
        auto prod = a;
        prod *= b;
        CHECK_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        CHECK_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION("multiply quad numbers with operator*= (4)") {
        constexpr auto a = quad(0.0);
        constexpr auto b = quad(0.0);
        constexpr auto prod_true = quad(0.0);
        auto prod = a;
        prod *= b;
        CHECK_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        CHECK_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION("multiply quad numbers with operator*") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0X0.61E3F58931EC2p-52);
        // 0x2.3f53c6a82f11712dad9c5fd34db6p+0
        constexpr auto prod_true =
            quad(0x2.3F53C6A82F118p+0, 0x0.12DAD9C5FD34Dp-52 - 0x1.0p-52);
        const auto prod = a * b;
        CHECK_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        CHECK_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION("multiply quad numbers with operator* (random)") {
        quad a;
        quad b;
        quad prod_true;
        std::tie(a, b,
            prod_true) = GENERATE(Catch::Generators::table<quad, quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.52b527b6c46a6p-17, 0x1.fb1f8716820c8p-71),
                quad(0x1.3735a527aa3a0p-8, 0x1.a73277b5f0aa0p-65),
                quad(0x1.9bc10d437ededp-25, 0x1.1d53201541e30p-80)),
            std::make_tuple(quad(0x1.15c15c22faf99p+7, 0x1.eaf4b47590204p-47),
                quad(-0x1.ae523e28a9262p-24, 0x1.2b1017f83acb8p-79),
                quad(-0x1.d2e4042336d08p-17, 0x1.54a54ed852200p-75)),
            std::make_tuple(
                quad(-0x1.1246ca6607d7ep-47, -0x1.42dce60279808p-102),
                quad(0x1.323833e7757d9p+30, 0x1.5b3f99313c7a0p-26),
                quad(-0x1.4814d50651c5ap-17, 0x1.ca19b15520790p-72)),
            std::make_tuple(quad(-0x1.cdfd31e1eeaafp+14, 0x1.6fa0de42b0780p-43),
                quad(-0x1.582805a6a9cfcp+5, 0x1.1c37331fcfdacp-49),
                quad(0x1.368a3a650d4eep+20, 0x1.a30f167d79cb8p-35)),
            // cspell: enable
            // NOLINTEND
        }));

        const auto prod = a * b;
        CHECK_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        CHECK_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION("multiply a double numbers with operator* (random)") {
        quad a;
        double b{};
        quad prod_true;
        std::tie(a, b,
            prod_true) = GENERATE(Catch::Generators::table<quad, double, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.58645c21bbf0ep-16, -0x1.647a8b7ec08c8p-71),
                0x1.c17cdbf807483p+16,
                quad(0x1.2e57ff0fa8674p+1, -0x1.7211371c224dcp-53)),
            std::make_tuple(quad(-0x1.7986766143f6ap+0, 0x1.19615589dc270p-56),
                0x1.4e79b4fedfba5p-31,
                quad(-0x1.ed40e9eb4339ap-31, 0x1.0434d99809590p-86)),
            std::make_tuple(quad(0x1.fd5d247a03357p-30, -0x1.09e10a59a3ca8p-85),
                -0x1.5a195eb27320ap+24,
                quad(-0x1.58512ee85dbd4p-5, 0x1.ba3127a308918p-59)),
            std::make_tuple(quad(0x1.57b12ae1c238dp-25, -0x1.d8caf920c9978p-80),
                0x1.1a52be8fe7d1ap+24,
                quad(0x1.7b083fd31d479p-1, 0x1.697aaad632d6cp-55)),
            // cspell: enable
            // NOLINTEND
        }));

        SECTION("quad * double") {
            const quad prod = a * b;
            CHECK_THAT(
                prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
            CHECK_THAT(prod.low(),
                Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
        }

        SECTION("double * quad") {
            const quad prod = b * a;
            CHECK_THAT(
                prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
            CHECK_THAT(prod.low(),
                Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
        }
    }

    SECTION(
        "divide a quad number with another quad number with operator/= (1)") {
        constexpr auto a = quad(0x1.0p+3, 0x0.3p-49);
        constexpr auto b = quad(0x1.0p+1, 0x0.1p-51);
        constexpr auto quotient_true = quad(0x1.0p+2, 0x0.2p-50);
        auto quotient = a;
        quotient /= b;
        CHECK_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        CHECK_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }

    SECTION(
        "divide a quad number with another quad number with operator/= (2)") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0x0.61E3F58931EC2p-52);
        // 0x1.bef0545a14fda729d96fa6624b59p+0
        constexpr auto quotient_true =
            quad(0x1.BEF0545A14FDAp+0, 0x0.729D96FA6624Bp-52);
        auto quotient = a;
        quotient /= b;
        CHECK_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        CHECK_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }

    SECTION(
        "divide a quad number with another quad number with operator/= (3)") {
        constexpr auto a = quad(0.0);
        constexpr auto b = quad(1.0);
        constexpr auto quotient_true = quad(0.0);
        auto quotient = a;
        quotient /= b;
        CHECK_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        CHECK_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }

    SECTION("divide a quad number with another quad number with operator/") {
        // 0x1.fb15fa86d92b228b6596fdb93fp+0
        constexpr auto a = quad(0x1.FB15FA86D92B2p+0, 0x0.28B6596FDB93Fp-52);
        // 0x1.22738c56c3ecf61e3f58931ec2p+0
        constexpr auto b = quad(0x1.22738C56C3ECFp+0, 0x0.61E3F58931EC2p-52);
        // 0x1.bef0545a14fda729d96fa6624b59p+0
        constexpr auto quotient_true =
            quad(0x1.BEF0545A14FDAp+0, 0x0.729D96FA6624Bp-52);
        const auto quotient = a / b;
        CHECK_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        CHECK_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }

    SECTION(
        "divide a quad number with another quad number with operator/ "
        "(random)") {
        quad a;
        quad b;
        quad quotient_true;
        std::tie(a, b, quotient_true) = GENERATE(Catch::Generators::table<quad,
            quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.52b527b6c46a6p-17, 0x1.fb1f8716820c8p-71),
                quad(0x1.3735a527aa3a0p-8, 0x1.a73277b5f0aa0p-65),
                quad(0x1.169eadc77cd88p-9, -0x1.3d48104d8bd30p-65)),
            std::make_tuple(quad(0x1.15c15c22faf99p+7, 0x1.eaf4b47590204p-47),
                quad(-0x1.ae523e28a9262p-24, 0x1.2b1017f83acb8p-79),
                quad(-0x1.4a79c774bdd5fp+30, 0x1.ddab28f63a8a0p-27)),
            std::make_tuple(
                quad(-0x1.1246ca6607d7ep-47, -0x1.42dce60279808p-102),
                quad(0x1.323833e7757d9p+30, 0x1.5b3f99313c7a0p-26),
                quad(-0x1.ca97579a08ca7p-78, -0x1.a27d2f1891becp-132)),
            std::make_tuple(quad(-0x1.cdfd31e1eeaafp+14, 0x1.6fa0de42b0780p-43),
                quad(-0x1.582805a6a9cfcp+5, 0x1.1c37331fcfdacp-49),
                quad(0x1.57a64da09df73p+9, -0x1.092e7b3636600p-45)),
            // cspell: enable
            // NOLINTEND
        }));

        const auto quotient = a / b;
        CHECK_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        CHECK_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }

    SECTION(
        "divide a quad number with a double number with operator/ "
        "(random)") {
        quad a;
        double b{};
        quad quotient_true;
        std::tie(a, b, quotient_true) = GENERATE(Catch::Generators::table<quad,
            double, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(0x1.58645c21bbf0ep-16, -0x1.647a8b7ec08c8p-71),
                0x1.c17cdbf807483p+16,
                quad(0x1.8849c5889cd9cp-33, -0x1.b6e77ae9da930p-88)),
            std::make_tuple(quad(-0x1.7986766143f6ap+0, 0x1.19615589dc270p-56),
                0x1.4e79b4fedfba5p-31,
                quad(-0x1.20f30a20e41fep+31, -0x1.806aad863d564p-23)),
            std::make_tuple(quad(0x1.fd5d247a03357p-30, -0x1.09e10a59a3ca8p-85),
                -0x1.5a195eb27320ap+24,
                quad(-0x1.78c32f63353fcp-54, 0x1.1b9b2eef5e3e0p-110)),
            std::make_tuple(quad(0x1.57b12ae1c238dp-25, -0x1.d8caf920c9978p-80),
                0x1.1a52be8fe7d1ap+24,
                quad(0x1.37a59dd1cb802p-49, -0x1.455c6773ec334p-103)),
            // cspell: enable
            // NOLINTEND
        }));

        const auto quotient = a / b;
        CHECK_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        CHECK_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }

    SECTION(
        "divide a double number with a quad number with operator/ "
        "(random)") {
        double a{};
        quad b;
        quad quotient_true;
        std::tie(a, b, quotient_true) =
            GENERATE(Catch::Generators::table<double, quad, quad>({
                // NOLINTBEGIN
                // cspell: disable
                std::make_tuple(0x1.c17cdbf807483p+16,
                    quad(0x1.58645c21bbf0ep-16, -0x1.647a8b7ec08c8p-71),
                    quad(0x1.4e1f2956cfc7bp+32, 0x1.c418fb77c8eb0p-22)),
                std::make_tuple(0x1.4e79b4fedfba5p-31,
                    quad(-0x1.7986766143f6ap+0, 0x1.19615589dc270p-56),
                    quad(-0x1.c59da4d01ac0ep-32, 0x1.e60a4bc569dd8p-86)),
                std::make_tuple(-0x1.5a195eb27320ap+24,
                    quad(0x1.fd5d247a03357p-30, -0x1.09e10a59a3ca8p-85),
                    quad(-0x1.5be3eae3b04c7p+53, 0x1.8b741080a1ff0p-3)),
                std::make_tuple(0x1.1a52be8fe7d1ap+24,
                    quad(0x1.57b12ae1c238dp-25, -0x1.d8caf920c9978p-80),
                    quad(0x1.a4941817daa49p+48, -0x1.d5fc65949fabcp-6)),
                // cspell: enable
                // NOLINTEND
            }));

        const auto quotient = a / b;
        CHECK_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        CHECK_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }

    SECTION("compare two quad numbers when higher digits differs") {
        const auto a = quad(0x1.0000000000000p+0, 0x1.0p-53);
        const auto b = quad(0x1.0000000000001p+0, 0x1.0p-53);

        CHECK(a < b);
        CHECK(a <= b);
        CHECK_FALSE(a > b);
        CHECK_FALSE(a >= b);
        CHECK_FALSE(a == b);
        CHECK(a != b);

        CHECK_FALSE(b < a);
        CHECK_FALSE(b <= a);
        CHECK(b > a);
        CHECK(b >= a);
        CHECK_FALSE(b == a);
        CHECK(b != a);
    }

    SECTION(
        "compare two quad numbers with same higher digits and different lower "
        "digits") {
        const auto a = quad(0x1.0p+0, 0x1.0000000000000p-53);
        const auto b = quad(0x1.0p+0, 0x1.0000000000001p-53);

        CHECK(a < b);
        CHECK(a <= b);
        CHECK_FALSE(a > b);
        CHECK_FALSE(a >= b);
        CHECK_FALSE(a == b);
        CHECK(a != b);

        CHECK_FALSE(b < a);
        CHECK_FALSE(b <= a);
        CHECK(b > a);
        CHECK(b >= a);
        CHECK_FALSE(b == a);
        CHECK(b != a);
    }

    SECTION("compare two quad numbers which are equal") {
        const auto a = quad(0x1.0p+0, 0x1.0p-53);
        const auto b = quad(0x1.0p+0, 0x1.0p-53);

        CHECK_FALSE(a < b);
        CHECK(a <= b);
        CHECK_FALSE(a > b);
        CHECK(a >= b);
        CHECK(a == b);
        CHECK_FALSE(a != b);

        CHECK_FALSE(b < a);
        CHECK(b <= a);
        CHECK_FALSE(b > a);
        CHECK(b >= a);
        CHECK(b == a);
        CHECK_FALSE(b != a);
    }

    SECTION("compare a quad number with a double number") {
        const auto a = quad(0x1.0000000000000p+0, 0x1.0p-53);
        const auto b = 0x1.0000000000001p+0;

        CHECK(a < b);
        CHECK(a <= b);
        CHECK_FALSE(a > b);
        CHECK_FALSE(a >= b);
        CHECK_FALSE(a == b);
        CHECK(a != b);

        CHECK_FALSE(b < a);
        CHECK_FALSE(b <= a);
        CHECK(b > a);
        CHECK(b >= a);
        CHECK_FALSE(b == a);
        CHECK(b != a);
    }
}
