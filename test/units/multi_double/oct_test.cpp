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
