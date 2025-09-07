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
 * \brief Test for gaussian_m1_rbf class.
 */
#include "num_collect/rbf/rbfs/gaussian_m1_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/differentiable_rbf.h"
#include "num_collect/rbf/concepts/fourth_order_differentiable_rbf.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/concepts/second_order_differentiable_rbf.h"
#include "num_collect/rbf/concepts/third_order_differentiable_rbf.h"
#include "num_collect/rbf/rbfs/differentiated.h"

TEST_CASE("num_collect::rbf::rbfs::gaussian_m1_rbf") {
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::differentiated_t;
    using num_collect::rbf::rbfs::gaussian_m1_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<gaussian_m1_rbf<double>>);
        STATIC_REQUIRE(rbf<gaussian_m1_rbf<float>>);
        STATIC_REQUIRE(num_collect::rbf::concepts::differentiable_rbf<
            gaussian_m1_rbf<double>>);
        STATIC_REQUIRE(
            num_collect::rbf::concepts::second_order_differentiable_rbf<
                gaussian_m1_rbf<double>>);
        STATIC_REQUIRE(
            num_collect::rbf::concepts::third_order_differentiable_rbf<
                gaussian_m1_rbf<double>>);
        STATIC_REQUIRE(
            num_collect::rbf::concepts::fourth_order_differentiable_rbf<
                gaussian_m1_rbf<double>>);
    }

    SECTION("calculate values") {
        const gaussian_m1_rbf<double> rbf;

        // Values are calculated in gaussian_m1_rbf_calc.py script.
        constexpr double tol_error = 1e-10;
        CHECK_THAT(rbf(0.0), Catch::Matchers::WithinAbs(0.0, tol_error));
        // NOLINTNEXTLINE(*-magic-numbers)
        CHECK_THAT(rbf(1e-6),
            Catch::Matchers::WithinRel(-9.999999999995e-13, tol_error));
        // NOLINTNEXTLINE(*-magic-numbers)
        CHECK_THAT(rbf(0.1),
            Catch::Matchers::WithinRel(-0.009950166250831946, tol_error));
        // NOLINTNEXTLINE(*-magic-numbers)
        CHECK_THAT(rbf(0.5),
            Catch::Matchers::WithinRel(-0.2211992169285951, tol_error));
        // NOLINTNEXTLINE(*-magic-numbers)
        CHECK_THAT(rbf(0.9),
            Catch::Matchers::WithinRel(-0.5551419337770589, tol_error));
        // NOLINTNEXTLINE(*-magic-numbers)
        CHECK_THAT(rbf(1.0),
            Catch::Matchers::WithinRel(-0.6321205588285577, tol_error));
        // NOLINTNEXTLINE(*-magic-numbers)
        CHECK_THAT(rbf(1.5),
            Catch::Matchers::WithinRel(-0.8946007754381357, tol_error));
    }

    SECTION("calculate a derivative") {
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.4738555174;
        const differentiated_t<gaussian_m1_rbf<double>> differentiated_rbf;

        const double value = differentiated_rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }

    SECTION("calculate a second derivative") {
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.9477110347;
        const differentiated_t<differentiated_t<gaussian_m1_rbf<double>>>
            differentiated_rbf;

        const double value = differentiated_rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }
}
