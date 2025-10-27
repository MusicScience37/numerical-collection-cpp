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
 * \brief Test of oct class.
 */
#include "num_collect/multi_double/oct.h"

#include <array>
#include <cmath>
#include <cstddef>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "format_oct_for_test.h"
#include "num_collect/multi_double/quad.h"

TEST_CASE("num_collect::multi_double::oct") {
    using num_collect::multi_double::oct;
    using num_collect::multi_double::quad;

    SECTION("default constructor") {
        const oct num;

        CHECK(num.term(0) == 0.0);
        CHECK(num.term(1) == 0.0);
        CHECK(num.term(2) == 0.0);
        CHECK(num.term(3) == 0.0);
    }

    SECTION("constructor with an array of terms") {
        constexpr auto terms_array = std::array<double, 4>{1.0, 2.0, 3.0, 4.0};

        const oct num(terms_array);

        CHECK(num.term(0) == 1.0);
        CHECK(num.term(1) == 2.0);
        CHECK(num.term(2) == 3.0);
        CHECK(num.term(3) == 4.0);
    }

    SECTION("constructor with terms") {
        const oct num(1.0, 2.0, 3.0, 4.0);

        CHECK(num.term(0) == 1.0);
        CHECK(num.term(1) == 2.0);
        CHECK(num.term(2) == 3.0);
        CHECK(num.term(3) == 4.0);
    }

    SECTION("constructor with double") {
        const oct num(1.5);

        CHECK(num.term(0) == 1.5);
        CHECK(num.term(1) == 0.0);
        CHECK(num.term(2) == 0.0);
        CHECK(num.term(3) == 0.0);
    }

    SECTION("constructor with quad") {
        const quad quad_num(2.5, 1.5);

        const oct num(quad_num);

        CHECK(num.term(0) == 2.5);
        CHECK(num.term(1) == 1.5);
        CHECK(num.term(2) == 0.0);
        CHECK(num.term(3) == 0.0);
    }

    SECTION("negate a number") {
        const oct num(0x1.0p+0, -0x1.0p-53, -0x1.0p-106, 0x1.0p-159);

        const oct neg_num = -num;

        CHECK(neg_num.term(0) == -0x1.0p+0);
        CHECK(neg_num.term(1) == 0x1.0p-53);
        CHECK(neg_num.term(2) == 0x1.0p-106);
        CHECK(neg_num.term(3) == -0x1.0p-159);
    }

    SECTION("add oct to oct at runtime") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(
                oct(0x1.96179cb334bc3p-8, 0x1.410aea50a8609p-63,
                    -0x1.dc61650752178p-119, -0x1.c877d80cd5a00p-173),
                oct(0x1.a80aa52d84e0fp-32, 0x1.9b82745fe1ae1p-89,
                    0x1.98ae81a4ae4a7p-143, -0x1.65a21f469be00p-197),
                oct(0x1.96179e5b3f616p-8, 0x1.bdb15bc47ed31p-70,
                    -0x1.faefb751caf14p-126, 0x1.8d6146c420000p-180)),
            std::make_tuple(
                oct(0x1.ac9e3ae780628p-13, -0x1.774892a4c52fap-68,
                    -0x1.b780d62ef948fp-122, 0x1.3700688b8ca80p-176),
                oct(0x1.33cc1fd9d87abp-12, 0x1.37be720f182efp-67,
                    -0x1.3da5094b40d91p-121, 0x1.e86a168b51980p-177),
                oct(0x1.050d9ea6cc560p-11, -0x1.e0f975d0d29a4p-65,
                    0x1.79a6a2e750a0ap-119, -0x1.d4ca8c2eca900p-176)),
            std::make_tuple(
                oct(0x1.7ef5f07995547p+3, -0x1.fb22a325fa712p-51,
                    0x1.573318b2e8163p-107, -0x1.5b06031d45400p-163),
                oct(-0x1.ad9874de18cdcp+37, -0x1.3e0eb7f68d4ffp-17,
                    0x1.440e7cdebbc48p-71, -0x1.4361d93087ed0p-125),
                oct(-0x1.ad9874ddb9105p+37, 0x1.c98a9d4ff3e77p-17,
                    -0x1.dca254665d90ap-72, -0x1.c12c71af9ab20p-126)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        const auto outputs = [] {
            std::array<oct, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, _] = inputs[i];
                results[i] = a + b;
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, result_true] = inputs[i];
            const auto result = outputs[i];
            const double abs_tol =
                (std::abs(a.term(0)) + std::abs(b.term(0))) * rel_tol;
            INFO("a = " << format_oct_for_test(a));
            INFO("b = " << format_oct_for_test(b));
            INFO("result_true = " << format_oct_for_test(result_true));
            INFO("result = " << format_oct_for_test(result));

            CHECK_THAT(result.term(0),
                Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
            CHECK_THAT(result.term(1),
                Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
            CHECK_THAT(result.term(2),
                Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
            CHECK_THAT(result.term(3),
                Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
        }
    }

    SECTION("add oct to oct at compile time") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(
                oct(0x1.96179cb334bc3p-8, 0x1.410aea50a8609p-63,
                    -0x1.dc61650752178p-119, -0x1.c877d80cd5a00p-173),
                oct(0x1.a80aa52d84e0fp-32, 0x1.9b82745fe1ae1p-89,
                    0x1.98ae81a4ae4a7p-143, -0x1.65a21f469be00p-197),
                oct(0x1.96179e5b3f616p-8, 0x1.bdb15bc47ed31p-70,
                    -0x1.faefb751caf14p-126, 0x1.8d6146c420000p-180)),
            std::make_tuple(
                oct(0x1.ac9e3ae780628p-13, -0x1.774892a4c52fap-68,
                    -0x1.b780d62ef948fp-122, 0x1.3700688b8ca80p-176),
                oct(0x1.33cc1fd9d87abp-12, 0x1.37be720f182efp-67,
                    -0x1.3da5094b40d91p-121, 0x1.e86a168b51980p-177),
                oct(0x1.050d9ea6cc560p-11, -0x1.e0f975d0d29a4p-65,
                    0x1.79a6a2e750a0ap-119, -0x1.d4ca8c2eca900p-176)),
            std::make_tuple(
                oct(0x1.7ef5f07995547p+3, -0x1.fb22a325fa712p-51,
                    0x1.573318b2e8163p-107, -0x1.5b06031d45400p-163),
                oct(-0x1.ad9874de18cdcp+37, -0x1.3e0eb7f68d4ffp-17,
                    0x1.440e7cdebbc48p-71, -0x1.4361d93087ed0p-125),
                oct(-0x1.ad9874ddb9105p+37, 0x1.c98a9d4ff3e77p-17,
                    -0x1.dca254665d90ap-72, -0x1.c12c71af9ab20p-126)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        constexpr auto outputs = [] {
            std::array<oct, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, _] = inputs[i];
                results[i] = a + b;
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, result_true] = inputs[i];
            const auto result = outputs[i];
            const double abs_tol =
                (std::abs(a.term(0)) + std::abs(b.term(0))) * rel_tol;
            INFO("a = " << format_oct_for_test(a));
            INFO("b = " << format_oct_for_test(b));
            INFO("result_true = " << format_oct_for_test(result_true));
            INFO("result = " << format_oct_for_test(result));

            CHECK_THAT(result.term(0),
                Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
            CHECK_THAT(result.term(1),
                Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
            CHECK_THAT(result.term(2),
                Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
            CHECK_THAT(result.term(3),
                Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
        }
    }

    SECTION("add double to oct at runtime") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(oct(0x1.831a1fd9362bfp+18, 0x1.45e43dd2979ccp-38,
                                0x1.8f2b9eb096b5ap-92, 0x1.e045cf7aea680p-146),
                0x1.de60caa1ccaf4p+21,
                oct(0x1.0762074e79ba6p+22, -0x1.d7437845ad0c6p-35,
                    -0x1.ce1a8c29ed295p-89, 0x1.3c08b9ef5d500p-143)),
            std::make_tuple(
                oct(0x1.98f107a10da81p-42, -0x1.434beb493c597p-96,
                    -0x1.0bb9c5b4a0147p-153, 0x1.769dcaccd9a00p-209),
                -0x1.3b3dcd03b2853p+37,
                oct(-0x1.3b3dcd03b2853p+37, 0x1.98f107a10da81p-42,
                    -0x1.434beb493c597p-96, -0x1.0bb9c00000000p-153)),
            std::make_tuple(oct(-0x1.54d3750b03757p+23, 0x1.2493388414c18p-31,
                                -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140),
                0x1.13e1e3e291a63p-22,
                oct(-0x1.54d3750b036cdp+23, 0x1.d0b5fb4ec2430p-32,
                    -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140)),
            std::make_tuple(
                oct(-0x1.cf6d0948a67a1p-18, -0x1.e0e9f6aaefb02p-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181),
                -0x1.565a87f6ed41fp-31,
                oct(-0x1.cf77bc1ce6318p-18, 0x1.17560955104fep-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        SECTION("oct + double") {
            const auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = a + b;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }

        SECTION("double + oct") {
            const auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = b + a;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }
    }

    SECTION("add double to oct at compile time") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(oct(0x1.831a1fd9362bfp+18, 0x1.45e43dd2979ccp-38,
                                0x1.8f2b9eb096b5ap-92, 0x1.e045cf7aea680p-146),
                0x1.de60caa1ccaf4p+21,
                oct(0x1.0762074e79ba6p+22, -0x1.d7437845ad0c6p-35,
                    -0x1.ce1a8c29ed295p-89, 0x1.3c08b9ef5d500p-143)),
            std::make_tuple(
                oct(0x1.98f107a10da81p-42, -0x1.434beb493c597p-96,
                    -0x1.0bb9c5b4a0147p-153, 0x1.769dcaccd9a00p-209),
                -0x1.3b3dcd03b2853p+37,
                oct(-0x1.3b3dcd03b2853p+37, 0x1.98f107a10da81p-42,
                    -0x1.434beb493c597p-96, -0x1.0bb9c00000000p-153)),
            std::make_tuple(oct(-0x1.54d3750b03757p+23, 0x1.2493388414c18p-31,
                                -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140),
                0x1.13e1e3e291a63p-22,
                oct(-0x1.54d3750b036cdp+23, 0x1.d0b5fb4ec2430p-32,
                    -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140)),
            std::make_tuple(
                oct(-0x1.cf6d0948a67a1p-18, -0x1.e0e9f6aaefb02p-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181),
                -0x1.565a87f6ed41fp-31,
                oct(-0x1.cf77bc1ce6318p-18, 0x1.17560955104fep-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        SECTION("oct + double") {
            constexpr auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = a + b;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }

        SECTION("double + oct") {
            constexpr auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = b + a;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }
    }

    SECTION("subtract oct from oct at runtime") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(
                oct(0x1.96179cb334bc3p-8, 0x1.410aea50a8609p-63,
                    -0x1.dc61650752178p-119, -0x1.c877d80cd5a00p-173),
                oct(0x1.a80aa52d84e0fp-32, 0x1.9b82745fe1ae1p-89,
                    0x1.98ae81a4ae4a7p-143, -0x1.65a21f469be00p-197),
                oct(0x1.96179b0b2a170p-8, 0x1.3f4d38f4e3e1cp-62,
                    -0x1.3a999d5400132p-116, -0x1.5e814e54e6700p-170)),
            std::make_tuple(
                oct(0x1.ac9e3ae780628p-13, -0x1.774892a4c52fap-68,
                    -0x1.b780d62ef948fp-122, 0x1.3700688b8ca80p-176),
                oct(0x1.33cc1fd9d87abp-12, 0x1.37be720f182efp-67,
                    -0x1.3da5094b40d91p-121, 0x1.e86a168b51980p-177),
                oct(-0x1.75f409986125dp-14, 0x1.93a893d0a7283p-72,
                    0x1.e49e33c434988p-129, 0x1.65aea2f1ee000p-183)),
            std::make_tuple(
                oct(0x1.7ef5f07995547p+3, -0x1.fb22a325fa712p-51,
                    0x1.573318b2e8163p-107, -0x1.5b06031d45400p-163),
                oct(-0x1.ad9874de18cdcp+37, -0x1.3e0eb7f68d4ffp-17,
                    0x1.440e7cdebbc48p-71, -0x1.4361d93087ed0p-125),
                oct(0x1.ad9874de788b4p+37, 0x1.16a034f43a1d1p-19,
                    -0x1.d9b88fc299452p-73, -0x1.674a19daf5fc0p-127)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        const auto outputs = [] {
            std::array<oct, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, _] = inputs[i];
                results[i] = a - b;
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, result_true] = inputs[i];
            const auto result = outputs[i];
            const double abs_tol =
                (std::abs(a.term(0)) + std::abs(b.term(0))) * rel_tol;
            INFO("a = " << format_oct_for_test(a));
            INFO("b = " << format_oct_for_test(b));
            INFO("result_true = " << format_oct_for_test(result_true));
            INFO("result = " << format_oct_for_test(result));

            CHECK_THAT(result.term(0),
                Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
            CHECK_THAT(result.term(1),
                Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
            CHECK_THAT(result.term(2),
                Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
            CHECK_THAT(result.term(3),
                Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
        }
    }

    SECTION("subtract oct from oct at compile time") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(
                oct(0x1.96179cb334bc3p-8, 0x1.410aea50a8609p-63,
                    -0x1.dc61650752178p-119, -0x1.c877d80cd5a00p-173),
                oct(0x1.a80aa52d84e0fp-32, 0x1.9b82745fe1ae1p-89,
                    0x1.98ae81a4ae4a7p-143, -0x1.65a21f469be00p-197),
                oct(0x1.96179b0b2a170p-8, 0x1.3f4d38f4e3e1cp-62,
                    -0x1.3a999d5400132p-116, -0x1.5e814e54e6700p-170)),
            std::make_tuple(
                oct(0x1.ac9e3ae780628p-13, -0x1.774892a4c52fap-68,
                    -0x1.b780d62ef948fp-122, 0x1.3700688b8ca80p-176),
                oct(0x1.33cc1fd9d87abp-12, 0x1.37be720f182efp-67,
                    -0x1.3da5094b40d91p-121, 0x1.e86a168b51980p-177),
                oct(-0x1.75f409986125dp-14, 0x1.93a893d0a7283p-72,
                    0x1.e49e33c434988p-129, 0x1.65aea2f1ee000p-183)),
            std::make_tuple(
                oct(0x1.7ef5f07995547p+3, -0x1.fb22a325fa712p-51,
                    0x1.573318b2e8163p-107, -0x1.5b06031d45400p-163),
                oct(-0x1.ad9874de18cdcp+37, -0x1.3e0eb7f68d4ffp-17,
                    0x1.440e7cdebbc48p-71, -0x1.4361d93087ed0p-125),
                oct(0x1.ad9874de788b4p+37, 0x1.16a034f43a1d1p-19,
                    -0x1.d9b88fc299452p-73, -0x1.674a19daf5fc0p-127)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        constexpr auto outputs = [] {
            std::array<oct, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, _] = inputs[i];
                results[i] = a - b;
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, result_true] = inputs[i];
            const auto result = outputs[i];
            const double abs_tol =
                (std::abs(a.term(0)) + std::abs(b.term(0))) * rel_tol;
            INFO("a = " << format_oct_for_test(a));
            INFO("b = " << format_oct_for_test(b));
            INFO("result_true = " << format_oct_for_test(result_true));
            INFO("result = " << format_oct_for_test(result));

            CHECK_THAT(result.term(0),
                Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
            CHECK_THAT(result.term(1),
                Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
            CHECK_THAT(result.term(2),
                Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
            CHECK_THAT(result.term(3),
                Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
        }
    }

    SECTION("subtract double from oct at runtime") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(oct(0x1.831a1fd9362bfp+18, 0x1.45e43dd2979ccp-38,
                                0x1.8f2b9eb096b5ap-92, 0x1.e045cf7aea680p-146),
                0x1.de60caa1ccaf4p+21,
                oct(-0x1.adfd86a6a5e9cp+21, -0x1.d7437845ad0c6p-35,
                    -0x1.ce1a8c29ed295p-89, 0x1.3c08b9ef5d4c0p-143)),
            std::make_tuple(
                oct(0x1.98f107a10da81p-42, -0x1.434beb493c597p-96,
                    -0x1.0bb9c5b4a0147p-153, 0x1.769dcaccd9a00p-209),
                -0x1.3b3dcd03b2853p+37,
                oct(0x1.3b3dcd03b2853p+37, 0x1.98f107a10da81p-42,
                    -0x1.434beb493c597p-96, -0x1.0bb9c00000000p-153)),
            std::make_tuple(oct(-0x1.54d3750b03757p+23, 0x1.2493388414c18p-31,
                                -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140),
                0x1.13e1e3e291a63p-22,
                oct(-0x1.54d3750b037e1p+23, 0x1.60cb7360c8618p-31,
                    -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140)),
            std::make_tuple(
                oct(-0x1.cf6d0948a67a1p-18, -0x1.e0e9f6aaefb02p-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181),
                -0x1.565a87f6ed41fp-31,
                oct(-0x1.cf62567466c2bp-18, 0x1.936b04aa8827fp-72,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        SECTION("oct - double") {
            const auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = a - b;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }

        SECTION("double - oct") {
            const auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = -(b - a);
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }
    }

    SECTION("subtract double from oct at compile time") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(oct(0x1.831a1fd9362bfp+18, 0x1.45e43dd2979ccp-38,
                                0x1.8f2b9eb096b5ap-92, 0x1.e045cf7aea680p-146),
                0x1.de60caa1ccaf4p+21,
                oct(-0x1.adfd86a6a5e9cp+21, -0x1.d7437845ad0c6p-35,
                    -0x1.ce1a8c29ed295p-89, 0x1.3c08b9ef5d4c0p-143)),
            std::make_tuple(
                oct(0x1.98f107a10da81p-42, -0x1.434beb493c597p-96,
                    -0x1.0bb9c5b4a0147p-153, 0x1.769dcaccd9a00p-209),
                -0x1.3b3dcd03b2853p+37,
                oct(0x1.3b3dcd03b2853p+37, 0x1.98f107a10da81p-42,
                    -0x1.434beb493c597p-96, -0x1.0bb9c00000000p-153)),
            std::make_tuple(oct(-0x1.54d3750b03757p+23, 0x1.2493388414c18p-31,
                                -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140),
                0x1.13e1e3e291a63p-22,
                oct(-0x1.54d3750b037e1p+23, 0x1.60cb7360c8618p-31,
                    -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140)),
            std::make_tuple(
                oct(-0x1.cf6d0948a67a1p-18, -0x1.e0e9f6aaefb02p-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181),
                -0x1.565a87f6ed41fp-31,
                oct(-0x1.cf62567466c2bp-18, 0x1.936b04aa8827fp-72,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181)),
            // cspell: enable
        });
        constexpr double rel_tol =
            0x1.0p-211;  // Error bound in the original paper.

        SECTION("oct - double") {
            constexpr auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = a - b;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }

        SECTION("double - oct") {
            constexpr auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = -(b - a);
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) + std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }
    }

    SECTION("multiply oct by oct at runtime") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(
                oct(0x1.96179cb334bc3p-8, 0x1.410aea50a8609p-63,
                    -0x1.dc61650752178p-119, -0x1.c877d80cd5a00p-173),
                oct(0x1.a80aa52d84e0fp-32, 0x1.9b82745fe1ae1p-89,
                    0x1.98ae81a4ae4a7p-143, -0x1.65a21f469be00p-197),
                oct(0x1.5053ff3d2da36p-39, 0x1.de21267093bb6p-93,
                    -0x1.0ed069f1d85f4p-147, 0x1.4610d706c8830p-201)),
            std::make_tuple(
                oct(0x1.ac9e3ae780628p-13, -0x1.774892a4c52fap-68,
                    -0x1.b780d62ef948fp-122, 0x1.3700688b8ca80p-176),
                oct(0x1.33cc1fd9d87abp-12, 0x1.37be720f182efp-67,
                    -0x1.3da5094b40d91p-121, 0x1.e86a168b51980p-177),
                oct(0x1.01abc2073a92bp-24, -0x1.dc93071c53107p-78,
                    -0x1.b53d52674cceap-136, 0x1.d5787de5a0600p-191)),
            std::make_tuple(
                oct(0x1.7ef5f07995547p+3, -0x1.fb22a325fa712p-51,
                    0x1.573318b2e8163p-107, -0x1.5b06031d45400p-163),
                oct(-0x1.ad9874de18cdcp+37, -0x1.3e0eb7f68d4ffp-17,
                    0x1.440e7cdebbc48p-71, -0x1.4361d93087ed0p-125),
                oct(-0x1.41531a6b1b63dp+41, 0x1.1da03bd52c9d9p-15,
                    -0x1.77550a7016757p-69, 0x1.b8aae4d2e2940p-123)),
            // cspell: enable
        });
        constexpr double rel_tol = 0x1.0p-211;  // Empirically determined.

        const auto outputs = [] {
            std::array<oct, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, _] = inputs[i];
                results[i] = a * b;
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, result_true] = inputs[i];
            const auto result = outputs[i];
            const double abs_tol =
                (std::abs(a.term(0)) * std::abs(b.term(0))) * rel_tol;
            INFO("a = " << format_oct_for_test(a));
            INFO("b = " << format_oct_for_test(b));
            INFO("result_true = " << format_oct_for_test(result_true));
            INFO("result = " << format_oct_for_test(result));

            CHECK_THAT(result.term(0),
                Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
            CHECK_THAT(result.term(1),
                Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
            CHECK_THAT(result.term(2),
                Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
            CHECK_THAT(result.term(3),
                Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
        }
    }

    SECTION("multiply oct by oct at compile time") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(
                oct(0x1.96179cb334bc3p-8, 0x1.410aea50a8609p-63,
                    -0x1.dc61650752178p-119, -0x1.c877d80cd5a00p-173),
                oct(0x1.a80aa52d84e0fp-32, 0x1.9b82745fe1ae1p-89,
                    0x1.98ae81a4ae4a7p-143, -0x1.65a21f469be00p-197),
                oct(0x1.5053ff3d2da36p-39, 0x1.de21267093bb6p-93,
                    -0x1.0ed069f1d85f4p-147, 0x1.4610d706c8830p-201)),
            std::make_tuple(
                oct(0x1.ac9e3ae780628p-13, -0x1.774892a4c52fap-68,
                    -0x1.b780d62ef948fp-122, 0x1.3700688b8ca80p-176),
                oct(0x1.33cc1fd9d87abp-12, 0x1.37be720f182efp-67,
                    -0x1.3da5094b40d91p-121, 0x1.e86a168b51980p-177),
                oct(0x1.01abc2073a92bp-24, -0x1.dc93071c53107p-78,
                    -0x1.b53d52674cceap-136, 0x1.d5787de5a0600p-191)),
            std::make_tuple(
                oct(0x1.7ef5f07995547p+3, -0x1.fb22a325fa712p-51,
                    0x1.573318b2e8163p-107, -0x1.5b06031d45400p-163),
                oct(-0x1.ad9874de18cdcp+37, -0x1.3e0eb7f68d4ffp-17,
                    0x1.440e7cdebbc48p-71, -0x1.4361d93087ed0p-125),
                oct(-0x1.41531a6b1b63dp+41, 0x1.1da03bd52c9d9p-15,
                    -0x1.77550a7016757p-69, 0x1.b8aae4d2e2940p-123)),
            // cspell: enable
        });
        constexpr double rel_tol = 0x1.0p-211;  // Empirically determined.

        constexpr auto outputs = [] {
            std::array<oct, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, _] = inputs[i];
                results[i] = a * b;
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, result_true] = inputs[i];
            const auto result = outputs[i];
            const double abs_tol =
                (std::abs(a.term(0)) * std::abs(b.term(0))) * rel_tol;
            INFO("a = " << format_oct_for_test(a));
            INFO("b = " << format_oct_for_test(b));
            INFO("result_true = " << format_oct_for_test(result_true));
            INFO("result = " << format_oct_for_test(result));

            CHECK_THAT(result.term(0),
                Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
            CHECK_THAT(result.term(1),
                Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
            CHECK_THAT(result.term(2),
                Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
            CHECK_THAT(result.term(3),
                Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
        }
    }

    SECTION("multiply oct by double at runtime") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(oct(0x1.831a1fd9362bfp+18, 0x1.45e43dd2979ccp-38,
                                0x1.8f2b9eb096b5ap-92, 0x1.e045cf7aea680p-146),
                0x1.de60caa1ccaf4p+21,
                oct(0x1.69ae91d56555dp+40, -0x1.69f8bf6f2d002p-15,
                    0x1.bc5d5ca6ad027p-69, 0x1.bed12b4771b60p-123)),
            std::make_tuple(
                oct(0x1.98f107a10da81p-42, -0x1.434beb493c597p-96,
                    -0x1.0bb9c5b4a0147p-153, 0x1.769dcaccd9a00p-209),
                -0x1.3b3dcd03b2853p+37,
                oct(-0x1.f7934d50e8497p-5, 0x1.75c078b492045p-61,
                    -0x1.63c00771d8f0ap-115, -0x1.9447ae72d4a00p-169)),
            std::make_tuple(oct(-0x1.54d3750b03757p+23, 0x1.2493388414c18p-31,
                                -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140),
                0x1.13e1e3e291a63p-22,
                oct(-0x1.6f4be3f9d8c45p+1, 0x1.52b24d5f3ac33p-53,
                    -0x1.b5d5c67eca30ep-108, 0x1.48d5acfccce20p-162)),
            std::make_tuple(
                oct(-0x1.cf6d0948a67a1p-18, -0x1.e0e9f6aaefb02p-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181),
                -0x1.565a87f6ed41fp-31,
                oct(0x1.35dfc66ee90e9p-48, 0x1.ea04d0caa2145p-103,
                    0x1.8012580a49321p-157, 0x1.d4a9fddbe6200p-214)),
            // cspell: enable
        });
        constexpr double rel_tol = 0x1.0p-211;  // Empirically determined.

        SECTION("oct * double") {
            const auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = a * b;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) * std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }

        SECTION("double * oct") {
            const auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = b * a;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) * std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }
    }

    SECTION("multiply oct by double at compile time") {
        static constexpr auto inputs = std::to_array({
            // cspell: disable
            std::make_tuple(oct(0x1.831a1fd9362bfp+18, 0x1.45e43dd2979ccp-38,
                                0x1.8f2b9eb096b5ap-92, 0x1.e045cf7aea680p-146),
                0x1.de60caa1ccaf4p+21,
                oct(0x1.69ae91d56555dp+40, -0x1.69f8bf6f2d002p-15,
                    0x1.bc5d5ca6ad027p-69, 0x1.bed12b4771b60p-123)),
            std::make_tuple(
                oct(0x1.98f107a10da81p-42, -0x1.434beb493c597p-96,
                    -0x1.0bb9c5b4a0147p-153, 0x1.769dcaccd9a00p-209),
                -0x1.3b3dcd03b2853p+37,
                oct(-0x1.f7934d50e8497p-5, 0x1.75c078b492045p-61,
                    -0x1.63c00771d8f0ap-115, -0x1.9447ae72d4a00p-169)),
            std::make_tuple(oct(-0x1.54d3750b03757p+23, 0x1.2493388414c18p-31,
                                -0x1.14963b4238dfap-86, 0x1.ec663e0118a00p-140),
                0x1.13e1e3e291a63p-22,
                oct(-0x1.6f4be3f9d8c45p+1, 0x1.52b24d5f3ac33p-53,
                    -0x1.b5d5c67eca30ep-108, 0x1.48d5acfccce20p-162)),
            std::make_tuple(
                oct(-0x1.cf6d0948a67a1p-18, -0x1.e0e9f6aaefb02p-73,
                    0x1.4c667f7e170e1p-127, -0x1.b603698a5a720p-181),
                -0x1.565a87f6ed41fp-31,
                oct(0x1.35dfc66ee90e9p-48, 0x1.ea04d0caa2145p-103,
                    0x1.8012580a49321p-157, 0x1.d4a9fddbe6200p-214)),
            // cspell: enable
        });
        constexpr double rel_tol = 0x1.0p-211;  // Empirically determined.

        SECTION("oct * double") {
            constexpr auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = a * b;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) * std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }

        SECTION("double * oct") {
            constexpr auto outputs = [] {
                std::array<oct, inputs.size()> results{};
                for (std::size_t i = 0; i < inputs.size(); ++i) {
                    const auto& [a, b, _] = inputs[i];
                    results[i] = b * a;
                }
                return results;
            }();

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                INFO("i = " << i);
                const auto& [a, b, result_true] = inputs[i];
                const auto result = outputs[i];
                const double abs_tol =
                    (std::abs(a.term(0)) * std::abs(b)) * rel_tol;
                INFO("a = " << format_oct_for_test(a));
                INFO("b = " << format_oct_for_test(b));
                INFO("result_true = " << format_oct_for_test(result_true));
                INFO("result = " << format_oct_for_test(result));

                CHECK_THAT(result.term(0),
                    Catch::Matchers::WithinAbs(result_true.term(0), abs_tol));
                CHECK_THAT(result.term(1),
                    Catch::Matchers::WithinAbs(result_true.term(1), abs_tol));
                CHECK_THAT(result.term(2),
                    Catch::Matchers::WithinAbs(result_true.term(2), abs_tol));
                CHECK_THAT(result.term(3),
                    Catch::Matchers::WithinAbs(result_true.term(3), abs_tol));
            }
        }
    }
}
