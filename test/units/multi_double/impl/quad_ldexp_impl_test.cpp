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
 * \brief Test of ldexp_impl function for quad class.
 */
#include "num_collect/multi_double/impl/quad_ldexp_impl.h"

#include <catch2/catch_test_macros.hpp>

#include "../format_quad_for_test.h"  // IWYU pragma: keep
#include "../quad_approx.h"

TEST_CASE("num_collect::multi_double::impl::ldexp_impl") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::ldexp_impl;

    SECTION("calculate at runtime") {
        constexpr quad input =
            quad(0x1.39ab86e200830p+45, -0x1.fb55396e22028p-9);
        constexpr int exp = -20;
        constexpr quad expected =
            quad(0x1.39ab86e200830p+25, -0x1.fb55396e22028p-29);

        const quad actual = ldexp_impl(input, exp);

        constexpr double relative_tolerance = 0x1.0p-102;
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }

    SECTION("calculate at compile time") {
        constexpr quad input =
            quad(0x1.39ab86e200830p+45, -0x1.fb55396e22028p-9);
        constexpr int exp = -20;
        constexpr quad expected =
            quad(0x1.39ab86e200830p+25, -0x1.fb55396e22028p-29);

        constexpr quad actual = ldexp_impl(input, exp);

        constexpr double relative_tolerance = 0x1.0p-102;
        CHECK_THAT(actual, quad_within_rel(expected, relative_tolerance));
    }
}
