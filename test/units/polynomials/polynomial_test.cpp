/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of polynomial class.
 */
#include "num_collect/polynomials/polynomial.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

TEST_CASE("num_collect::polynomials::polynomial") {
    using num_collect::polynomials::polynomial;
    using num_collect::util::vector;

    SECTION("create a polynomial") {
        const auto poly = polynomial<double>({1.0, 2.0, 3.0});

        CHECK_THAT(
            poly.coeffs(), Catch::Matchers::RangeEquals(vector{1.0, 2.0, 3.0}));
    }

    SECTION("access coefficients via vector") {
        auto poly = polynomial<float>({0.5F, 1.5F});

        poly.coeffs().push_back(2.5F);

        CHECK_THAT(poly.coeffs(),
            Catch::Matchers::RangeEquals(vector{0.5F, 1.5F, 2.5F}));
    }

    SECTION("get coefficients for each degree") {
        const auto poly = polynomial<int>({1, 0, -1});

        CHECK(poly.coeffs()[0] == 1);
        CHECK(poly.coeffs()[1] == 0);
        CHECK(poly.coeffs()[2] == -1);
    }

    SECTION("access coefficients for each degree") {
        // NOLINTNEXTLINE(google-runtime-int): "L" suffix requires long type.
        auto poly = polynomial<long>({10L, 20L});

        poly.coeffs()[1] = 30L;

        CHECK(poly.coeffs()[0] == 10L);
        CHECK(poly.coeffs()[1] == 30L);
    }

    SECTION("get the degree of a polynomial") {
        const auto poly = polynomial<int>({1, 0, -1});

        CHECK(poly.degree() == 2);
    }

    SECTION("change the degree of a polynomial") {
        auto poly = polynomial<double>({1.0, 2.0, 3.0});

        poly.change_degree(1);

        CHECK_THAT(
            poly.coeffs(), Catch::Matchers::RangeEquals(vector{1.0, 2.0}));
    }

    SECTION("change the degree of a polynomial to -1") {
        auto poly = polynomial<float>({0.5F, 1.5F});

        poly.change_degree(-1);

        CHECK(poly.coeffs().empty());
    }

    SECTION("multiply polynomials using operator*") {
        const auto poly1 = polynomial<int>({1, 2});  // 1 + 2x
        const auto poly2 = polynomial<int>({3, 4});  // 3 + 4x

        const auto result = poly1 * poly2;  // (1 + 2x)(3 + 4x) = 3 + 10x + 8x^2

        CHECK_THAT(
            result.coeffs(), Catch::Matchers::RangeEquals(vector{3, 10, 8}));
    }

    SECTION("multiply polynomials using operator*=") {
        auto poly1 = polynomial<int>({1, 2});        // 1 + 2x
        const auto poly2 = polynomial<int>({3, 4});  // 3 + 4x

        poly1 *= poly2;  // (1 + 2x)(3 + 4x) = 3 + 10x + 8x^2

        CHECK_THAT(
            poly1.coeffs(), Catch::Matchers::RangeEquals(vector{3, 10, 8}));
    }

    SECTION("multiply an empty polynomial") {
        const auto empty_poly = polynomial<double>({});
        const auto poly = polynomial<double>({1.0, 2.0});

        const auto result1 = empty_poly * poly;
        const auto result2 = poly * empty_poly;

        CHECK(result1.coeffs().empty());
        CHECK(result2.coeffs().empty());
    }

    // Free functions.

    SECTION("differentiate a polynomial") {
        auto poly = polynomial<double>({1.0, 2.0, 3.0});  // 1 + 2x + 3x^2

        num_collect::polynomials::differentiate(poly);  // 2 + 6x

        CHECK_THAT(
            poly.coeffs(), Catch::Matchers::RangeEquals(vector{2.0, 6.0}));
    }
}
