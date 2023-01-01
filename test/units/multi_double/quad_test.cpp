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
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::multi_double::quad") {
    using num_collect::multi_double::quad;

    SECTION("construct without arguments") {
        quad num;
        REQUIRE_THAT(num.high(), Catch::Matchers::WithinULP(0.0, 0));
        REQUIRE_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    SECTION("construct with double argument") {
        constexpr double value = 3.14;
        quad num = value;
        REQUIRE_THAT(num.high(), Catch::Matchers::WithinULP(value, 0));
        REQUIRE_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    SECTION("construct with int argument") {
        constexpr int value = 37;
        quad num = value;
        REQUIRE_THAT(num.high(),
            Catch::Matchers::WithinULP(static_cast<double>(value), 0));
        REQUIRE_THAT(num.low(), Catch::Matchers::WithinULP(0.0, 0));
    }

    constexpr std::uint64_t ulp_limit = 8;

    SECTION("add a quad number with operator+= (1)") {
        constexpr auto a = quad(0x1.0000000000001p+0, 0x0.7000000000001p-52);
        constexpr auto b = quad(0x1.0000000000007p+0, 0x0.8000000000002p-52);
        constexpr auto sum_true =
            quad(0x2.0000000000008p+0, 0x0.F000000000003p-52);
        auto sum = a;
        sum += b;
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a quad number with operator+= (2)") {
        constexpr auto a = quad(0x1.0000000000000p+0, 0x0.7p-52);
        constexpr auto b = quad(0x0.8000000000000p+0, 0x0.2p-52);
        constexpr auto sum_true = quad(0x1.8000000000001p+0, -0x0.7p-52);
        auto sum = a;
        sum += b;
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("add a quad number with operator+= (3)") {
        constexpr auto a = quad(0x1.0p+0, 0x0.7p-52);
        constexpr auto b = quad(-0x1.0p+0, 0x0.2p-52);
        constexpr auto sum_true = quad(0x0.9p-52);
        auto sum = a;
        sum += b;
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(
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
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(
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
        REQUIRE_THAT(
            sum.high(), Catch::Matchers::WithinULP(sum_true.high(), 0));
        REQUIRE_THAT(
            sum.low(), Catch::Matchers::WithinULP(sum_true.low(), ulp_limit));
    }

    SECTION("subtract a quad number with operator-= (1)") {
        constexpr auto a = quad(0x1.0000000000001p+0, 0x0.7000000000001p-52);
        constexpr auto b = quad(-0x1.0000000000007p+0, -0x0.8000000000002p-52);
        constexpr auto dif_true =
            quad(0x2.0000000000008p+0, 0x0.F000000000003p-52);
        auto dif = a;
        dif -= b;
        REQUIRE_THAT(
            dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        REQUIRE_THAT(
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
        REQUIRE_THAT(
            dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        REQUIRE_THAT(
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
        REQUIRE_THAT(
            dif.high(), Catch::Matchers::WithinULP(dif_true.high(), 0));
        REQUIRE_THAT(
            dif.low(), Catch::Matchers::WithinULP(dif_true.low(), ulp_limit));
    }

    SECTION("multiply quad numbers with operator*= (1)") {
        constexpr auto a = quad(0x1.0p+1, 0x0.1p-51);
        constexpr auto b = quad(0x1.0p+2, 0x0.2p-50);
        constexpr auto prod_true = quad(0x1.0p+3, 0x0.3p-49);
        auto prod = a;
        prod *= b;
        REQUIRE_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        REQUIRE_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION("multiply quad numbers with operator*= (2)") {
        constexpr auto a = quad(0x1.0000001p+1);
        constexpr auto b = quad(0x1.0000001p+2);
        constexpr auto prod_true = quad(0x1.0000002p+3, 0x0.1p-49);
        auto prod = a;
        prod *= b;
        REQUIRE_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        REQUIRE_THAT(
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
        REQUIRE_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        REQUIRE_THAT(
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
        REQUIRE_THAT(
            prod.high(), Catch::Matchers::WithinULP(prod_true.high(), 0));
        REQUIRE_THAT(
            prod.low(), Catch::Matchers::WithinULP(prod_true.low(), ulp_limit));
    }

    SECTION(
        "divide a quad number with another quad number with operator/= (1)") {
        constexpr auto a = quad(0x1.0p+3, 0x0.3p-49);
        constexpr auto b = quad(0x1.0p+1, 0x0.1p-51);
        constexpr auto quotient_true = quad(0x1.0p+2, 0x0.2p-50);
        auto quotient = a;
        quotient /= b;
        REQUIRE_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        REQUIRE_THAT(quotient.low(),
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
        REQUIRE_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        REQUIRE_THAT(quotient.low(),
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
        REQUIRE_THAT(quotient.high(),
            Catch::Matchers::WithinULP(quotient_true.high(), 0));
        REQUIRE_THAT(quotient.low(),
            Catch::Matchers::WithinULP(quotient_true.low(), ulp_limit));
    }
}
