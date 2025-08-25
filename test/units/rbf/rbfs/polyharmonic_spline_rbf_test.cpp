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
 * \brief Test of polyharmonic_spline_rbf class.
 */
#include "num_collect/rbf/rbfs/polyharmonic_spline_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/rbf.h"

TEST_CASE("num_collect::rbf::rbfs::polyharmonic_spline_rbf") {
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::polyharmonic_spline_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<polyharmonic_spline_rbf<double, 1>>);
        STATIC_REQUIRE(rbf<polyharmonic_spline_rbf<float, 2>>);
    }

    constexpr double epsilon = 1e-7;

    SECTION("Degree 1") {
        const polyharmonic_spline_rbf<double, 1> rbf;

        // NOLINTNEXTLINE
        CHECK_THAT(rbf(0.0), Catch::Matchers::WithinAbs(0.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(0.5), Catch::Matchers::WithinAbs(0.5, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.0), Catch::Matchers::WithinAbs(1.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.5), Catch::Matchers::WithinAbs(1.5, epsilon));
    }

    SECTION("Degree 2") {
        const polyharmonic_spline_rbf<double, 2> rbf;

        // NOLINTNEXTLINE
        CHECK_THAT(rbf(0.0), Catch::Matchers::WithinAbs(0.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(
            rbf(0.5), Catch::Matchers::WithinAbs(-0.1732867951, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.0), Catch::Matchers::WithinAbs(0.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.5), Catch::Matchers::WithinAbs(0.9122964932, epsilon));
    }

    SECTION("Degree 3") {
        const polyharmonic_spline_rbf<double, 3> rbf;

        // NOLINTNEXTLINE
        CHECK_THAT(rbf(0.0), Catch::Matchers::WithinAbs(0.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(0.5), Catch::Matchers::WithinAbs(0.125, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.0), Catch::Matchers::WithinAbs(1.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.5), Catch::Matchers::WithinAbs(3.375, epsilon));
    }

    SECTION("Degree 4") {
        const polyharmonic_spline_rbf<double, 4> rbf;

        // NOLINTNEXTLINE
        CHECK_THAT(rbf(0.0), Catch::Matchers::WithinAbs(0.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(
            rbf(0.5), Catch::Matchers::WithinAbs(-0.04332169878, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.0), Catch::Matchers::WithinAbs(0.0, epsilon));
        // NOLINTNEXTLINE
        CHECK_THAT(rbf(1.5), Catch::Matchers::WithinAbs(2.05266711, epsilon));
    }
}
