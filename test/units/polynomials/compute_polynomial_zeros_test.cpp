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
 * \brief Test of compute_polynomial_zeros function.
 */
#include "num_collect/polynomials/compute_polynomial_zeros.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::polynomials::compute_zeros") {
    using num_collect::polynomials::compute_zeros;
    using num_collect::polynomials::polynomial;

    constexpr double tolerance = 1e-10;

    SECTION("compute zeros of a polynomial of degree 1") {
        const auto poly = polynomial<double>({-1.5, 2.0});  // -1.5 + 2x

        const auto zeros = compute_zeros(poly);

        CHECK(zeros.size() == 1);
        CHECK_THAT(
            zeros[0].real(), Catch::Matchers::WithinAbs(0.75, tolerance));
        CHECK_THAT(zeros[0].imag(), Catch::Matchers::WithinAbs(0.0, tolerance));
    }

    SECTION("compute zeros of a polynomial of degree 2") {
        const auto poly =
            polynomial<double>({1.0, -3.0, 2.0});  // 1 - 3x + 2x^2

        const auto zeros = compute_zeros(poly);

        CHECK(zeros.size() == 2);
        CHECK_THAT(zeros[0].real(), Catch::Matchers::WithinAbs(1.0, tolerance));
        CHECK_THAT(zeros[0].imag(), Catch::Matchers::WithinAbs(0.0, tolerance));
        CHECK_THAT(zeros[1].real(), Catch::Matchers::WithinAbs(0.5, tolerance));
        CHECK_THAT(zeros[1].imag(), Catch::Matchers::WithinAbs(0.0, tolerance));
    }
}
