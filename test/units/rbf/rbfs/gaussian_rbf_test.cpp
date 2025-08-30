/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of gaussian_rbf class.
 */
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/differentiable_rbf.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/rbfs/differentiated.h"

TEST_CASE("num_collect::rbf::rbfs::gaussian_rbf") {
    using num_collect::rbf::concepts::differentiable_rbf;
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::differentiated_t;
    using num_collect::rbf::rbfs::gaussian_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<gaussian_rbf<double>>);
        STATIC_REQUIRE(rbf<gaussian_rbf<float>>);
        STATIC_REQUIRE(differentiable_rbf<gaussian_rbf<double>>);
    }

    SECTION("calculate a value") {
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.2369277587;
        const gaussian_rbf<double> rbf;

        const double value = rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }

    SECTION("calculate a derivative") {
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.4738555174;
        const differentiated_t<gaussian_rbf<double>> differentiated_rbf;

        const double value = differentiated_rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }

    SECTION("calculate a second derivative") {
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.9477110347;
        const differentiated_t<differentiated_t<gaussian_rbf<double>>>
            differentiated_rbf;

        const double value = differentiated_rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }
}
