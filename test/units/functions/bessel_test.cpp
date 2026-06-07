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

TEMPLATE_TEST_CASE(
    "num_collect::functions::cyl_neumann", "", float, double, long double) {
    using Scalar = TestType;
    // Catch2 does not support long double in matchers, so use double for
    // comparison if Scalar is long double.
    using ComparisonType =
        std::conditional_t<std::is_same_v<Scalar, long double>, double, Scalar>;
    using num_collect::functions::cyl_neumann;

    SECTION("calculate") {
#define CHECK_NEUMANN(nu, x, expected)                                \
    CHECK_THAT(static_cast<ComparisonType>(cyl_neumann(               \
                   static_cast<Scalar>(nu), static_cast<Scalar>(x))), \
        Catch::Matchers::WithinRel(static_cast<ComparisonType>(expected)))

        // Generated using bessel_calc.py script in this directory.
        CHECK_NEUMANN(0.0, 0.001, -4.471416611375924);
        CHECK_NEUMANN(0.0, 0.3, -0.8072735778045197);
        CHECK_NEUMANN(0.0, 1.0, 0.088256964215677);
        CHECK_NEUMANN(0.0, 3.0, 0.37685001001279056);
        CHECK_NEUMANN(0.0, 10.0, 0.05567116728359934);
        CHECK_NEUMANN(1.0, 0.001, -636.6221672311394);
        CHECK_NEUMANN(1.0, 0.3, -2.2931051383885293);
        CHECK_NEUMANN(1.0, 1.0, -0.7812128213002889);
        CHECK_NEUMANN(1.0, 3.0, 0.32467442479180014);
        CHECK_NEUMANN(1.0, 10.0, 0.24901542420695386);
        CHECK_NEUMANN(7.0, 3.0, -19.839935408986413);
        CHECK_NEUMANN(1.2, 0.34, -2.6510542978237366);

#undef CHECK_NEUMANN
    }
}

TEMPLATE_TEST_CASE(
    "num_collect::functions::cyl_bessel_i", "", float, double, long double) {
    using Scalar = TestType;
    // Catch2 does not support long double in matchers, so use double for
    // comparison if Scalar is long double.
    using ComparisonType =
        std::conditional_t<std::is_same_v<Scalar, long double>, double, Scalar>;
    using num_collect::functions::cyl_bessel_i;

    SECTION("calculate") {
#define CHECK_BESSEL_I(nu, x, expected)                               \
    CHECK_THAT(static_cast<ComparisonType>(cyl_bessel_i(              \
                   static_cast<Scalar>(nu), static_cast<Scalar>(x))), \
        Catch::Matchers::WithinRel(static_cast<ComparisonType>(expected)))

        CHECK_BESSEL_I(0.0, 0.0, 1.0);
        CHECK_BESSEL_I(0.0, 0.001, 1.0000002500000158);
        CHECK_BESSEL_I(0.0, 0.3, 1.0226268793515971);
        CHECK_BESSEL_I(0.0, 1.0, 1.2660658777520084);
        CHECK_BESSEL_I(0.0, 3.0, 4.880792585865025);
        CHECK_BESSEL_I(0.0, 10.0, 2815.7166284662544);
        CHECK_BESSEL_I(1.0, 0.0, 0.0);
        CHECK_BESSEL_I(1.0, 0.001, 0.0005000000625000026);
        CHECK_BESSEL_I(1.0, 0.3, 0.15169384000359276);
        CHECK_BESSEL_I(1.0, 1.0, 0.565159103992485);
        CHECK_BESSEL_I(1.0, 3.0, 3.95337021740261);
        CHECK_BESSEL_I(1.0, 10.0, 2670.988303701254);
        CHECK_BESSEL_I(7.0, 3.0, 0.0044721187299495676);
        CHECK_BESSEL_I(1.2, 0.34, 0.10968022802535612);

#undef CHECK_BESSEL_I
    }
}

TEMPLATE_TEST_CASE(
    "num_collect::functions::cyl_bessel_k", "", float, double, long double) {
    using Scalar = TestType;
    // Catch2 does not support long double in matchers, so use double for
    // comparison if Scalar is long double.
    using ComparisonType =
        std::conditional_t<std::is_same_v<Scalar, long double>, double, Scalar>;
    using num_collect::functions::cyl_bessel_k;

    SECTION("calculate") {
#define CHECK_BESSEL_K(nu, x, expected)                               \
    CHECK_THAT(static_cast<ComparisonType>(cyl_bessel_k(              \
                   static_cast<Scalar>(nu), static_cast<Scalar>(x))), \
        Catch::Matchers::WithinRel(static_cast<ComparisonType>(expected)))

        CHECK_BESSEL_K(0.0, 0.001, 7.023688800562382);
        CHECK_BESSEL_K(0.0, 0.3, 1.3724600605442976);
        CHECK_BESSEL_K(0.0, 1.0, 0.42102443824070834);
        CHECK_BESSEL_K(0.0, 3.0, 0.03473950438627925);
        CHECK_BESSEL_K(0.0, 10.0, 1.778006231616765e-05);
        CHECK_BESSEL_K(1.0, 0.001, 999.9962381560855);
        CHECK_BESSEL_K(1.0, 0.3, 3.0559920334573247);
        CHECK_BESSEL_K(1.0, 1.0, 0.6019072301972346);
        CHECK_BESSEL_K(1.0, 3.0, 0.040156431128194184);
        CHECK_BESSEL_K(1.0, 10.0, 1.8648773453825585e-05);
        CHECK_BESSEL_K(7.0, 3.0, 14.664826474155348);
        CHECK_BESSEL_K(1.2, 0.34, 3.5758733761387407);

#undef CHECK_BESSEL_K
    }
}
