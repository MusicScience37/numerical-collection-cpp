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
 * \brief Test of inverse_multi_quadric_rbf class.
 */
#include "num_collect/rbf/rbfs/inverse_multi_quadric_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/rbfs/differentiated.h"

TEST_CASE("num_collect::rbf::rbfs::inverse_multi_quadric_rbf") {
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::differentiated_t;
    using num_collect::rbf::rbfs::inverse_multi_quadric_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<inverse_multi_quadric_rbf<double>>);
        STATIC_REQUIRE(rbf<inverse_multi_quadric_rbf<float>>);
    }

    SECTION("calculate a value") {
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.640184399664479;
        const inverse_multi_quadric_rbf<double> rbf;

        const double value = rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }

    SECTION("calculate a derivative") {
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.2623706556;
        const differentiated_t<inverse_multi_quadric_rbf<double>> rbf;

        const double value = rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }
}
