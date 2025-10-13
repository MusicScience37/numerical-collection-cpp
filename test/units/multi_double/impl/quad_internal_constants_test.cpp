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
 * \brief Test of internal constants of quad class.
 */
#include "num_collect/multi_double/impl/quad_internal_constants.h"

#include <catch2/catch_test_macros.hpp>

#include "../format_quad_for_test.h"  // IWYU pragma: keep
#include "../quad_approx.h"
#include "num_collect/multi_double/impl/quad_trigonometric_impl.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_math.h"

TEST_CASE("num_collect::multi_double::impl::log2_quad") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::log2_quad;

    SECTION("check exp(log2_quad) == 2") {
        // Use Maclaurin series here because exp function depends on this
        // constant.
        quad term = log2_quad;
        quad result = term;
        constexpr int num_terms = 200;
        for (int i = 2; i <= num_terms; ++i) {
            term *= log2_quad;
            term /= i;
            result += term;
        }
        result += quad(1.0);

        constexpr quad expected = quad(2.0);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(result, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::impl::log2_inv_quad") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::log2_inv_quad;
    using num_collect::multi_double::impl::log2_quad;

    SECTION("check log2_quad * log2_inv_quad == 1") {
        const quad result = log2_quad * log2_inv_quad;
        constexpr quad expected = quad(1.0);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(result, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::impl::log10_quad") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::log10_quad;

    SECTION("check exp(log10_quad) == 10") {
        // Use Maclaurin series here because exp function depends on this
        // constant.
        quad term = log10_quad;
        quad result = term;
        constexpr int num_terms = 200;
        for (int i = 2; i <= num_terms; ++i) {
            term *= log10_quad;
            term /= i;
            result += term;
        }
        result += quad(1.0);

        constexpr quad expected = quad(10.0);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(result, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::impl::log10_inv_quad") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::log10_inv_quad;
    using num_collect::multi_double::impl::log10_quad;

    SECTION("check log10_quad * log10_inv_quad == 1") {
        const quad result = log10_quad * log10_inv_quad;
        constexpr quad expected = quad(1.0);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(result, quad_within_rel(expected, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::impl::pi_over_4_quad") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::pi_over_4_quad;

    SECTION("check sin(pi_over_4) == sqrt(0.5)") {
        const quad left =
            num_collect::multi_double::impl::sin_maclaurin(pi_over_4_quad);
        const quad right = num_collect::multi_double::sqrt(quad(0.5));
        constexpr quad relative_tolerance(0x1.0p-100);
        CHECK_THAT(left, quad_within_rel(right, relative_tolerance));
    }
}

TEST_CASE("num_collect::multi_double::impl::pi_over_4_inv_quad") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::pi_over_4_inv_quad;
    using num_collect::multi_double::impl::pi_over_4_quad;

    SECTION("check pi_over_4_quad * pi_over_4_inv_quad == 1") {
        const quad result = pi_over_4_quad * pi_over_4_inv_quad;
        constexpr quad expected = quad(1.0);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(result, quad_within_rel(expected, relative_tolerance));
    }
}
