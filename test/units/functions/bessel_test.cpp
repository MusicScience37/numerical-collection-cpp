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
 * \brief Tests of functions to calculate Bessel functions.
 */
#include "num_collect/functions/bessel.h"

#include <type_traits>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEMPLATE_TEST_CASE(
    "num_collect::functions::cyl_bessel_j", "", float, double, long double) {
    using Scalar = TestType;
    // Catch2 does not support long double in matchers, so use double for
    // comparison if Scalar is long double.
    using ComparisonType =
        std::conditional_t<std::is_same_v<Scalar, long double>, double, Scalar>;
    using num_collect::functions::cyl_bessel_j;

    SECTION("calculate") {
#define CHECK_BESSEL_J(nu, x, expected)                               \
    CHECK_THAT(static_cast<ComparisonType>(cyl_bessel_j(              \
                   static_cast<Scalar>(nu), static_cast<Scalar>(x))), \
        Catch::Matchers::WithinRel(static_cast<ComparisonType>(expected)))

        // Generated using bessel_calc.py script in this directory.
        CHECK_BESSEL_J(0.0, 0.0, 1.0);
        CHECK_BESSEL_J(0.0, 0.001, 0.9999997500000156);
        CHECK_BESSEL_J(0.0, 0.3, 0.9776262465382961);
        CHECK_BESSEL_J(0.0, 1.0, 0.7651976865579666);
        CHECK_BESSEL_J(0.0, 3.0, -0.2600519549019334);
        CHECK_BESSEL_J(0.0, 10.0, -0.24593576445134832);
        CHECK_BESSEL_J(1.0, 0.0, 0.0);
        CHECK_BESSEL_J(1.0, 0.001, 0.0004999999375000028);
        CHECK_BESSEL_J(1.0, 0.3, 0.148318816273104);
        CHECK_BESSEL_J(1.0, 1.0, 0.44005058574493355);
        CHECK_BESSEL_J(1.0, 3.0, 0.33905895852593626);
        CHECK_BESSEL_J(1.0, 10.0, 0.0434727461688616);
        CHECK_BESSEL_J(7.0, 3.0, 0.002547294451804692);
        CHECK_BESSEL_J(1.2, 0.34, 0.10683612966820742);

#undef CHECK_BESSEL_J
    }
}
