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
 * \brief Test for gaussian_from_square_rbf class.
 */
#include "num_collect/rbf/rbfs/gaussian_from_square_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/differentiable_rbf.h"
#include "num_collect/rbf/concepts/fourth_order_differentiable_rbf.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/concepts/second_order_differentiable_rbf.h"
#include "num_collect/rbf/concepts/third_order_differentiable_rbf.h"
#include "num_collect/rbf/rbfs/differentiated.h"

TEST_CASE("num_collect::rbf::rbfs::gaussian_from_square_rbf") {
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::differentiated_t;
    using num_collect::rbf::rbfs::gaussian_from_square_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<gaussian_from_square_rbf<double>>);
        STATIC_REQUIRE(rbf<gaussian_from_square_rbf<float>>);
        STATIC_REQUIRE(num_collect::rbf::concepts::differentiable_rbf<
            gaussian_from_square_rbf<double>>);
        STATIC_REQUIRE(
            num_collect::rbf::concepts::second_order_differentiable_rbf<
                gaussian_from_square_rbf<double>>);
        STATIC_REQUIRE(
            num_collect::rbf::concepts::third_order_differentiable_rbf<
                gaussian_from_square_rbf<double>>);
        STATIC_REQUIRE(
            num_collect::rbf::concepts::fourth_order_differentiable_rbf<
                gaussian_from_square_rbf<double>>);
    }

    SECTION("calculate values") {
        const gaussian_from_square_rbf<double> rbf;

        // Values are calculated in gaussian_from_square_rbf_calc.py script.
        constexpr double tol_error = 1e-10;
        CHECK_THAT(rbf(0.0), Catch::Matchers::WithinAbs(0.0, tol_error));
        CHECK_THAT(rbf(1e-6),
            Catch::Matchers::WithinRel(4.999999999998333e-25, tol_error));
        CHECK_THAT(rbf(0.099),
            Catch::Matchers::WithinRel(4.787327086630015e-5, tol_error));
        CHECK_THAT(rbf(0.1),
            Catch::Matchers::WithinRel(4.983374916805357e-5, tol_error));
        CHECK_THAT(rbf(0.101),
            Catch::Matchers::WithinRel(5.185373074623816e-5, tol_error));
        CHECK_THAT(rbf(0.5),
            Catch::Matchers::WithinRel(0.02880078307140487, tol_error));
        CHECK_THAT(rbf(0.9),
            Catch::Matchers::WithinRel(0.2548580662229411, tol_error));
        CHECK_THAT(rbf(1.0),
            Catch::Matchers::WithinRel(0.3678794411714423, tol_error));
        CHECK_THAT(
            rbf(1.5), Catch::Matchers::WithinRel(1.355399224561864, tol_error));
    }

    SECTION("calculate derivatives") {
        const differentiated_t<gaussian_from_square_rbf<double>>
            differentiated_rbf;

        // Values are calculated in gaussian_from_square_rbf_calc.py script.
        constexpr double tol_error = 1e-10;
        CHECK_THAT(differentiated_rbf(0.0),
            Catch::Matchers::WithinAbs(0.0, tol_error));
        CHECK_THAT(differentiated_rbf(1e-6),
            Catch::Matchers::WithinRel(-1.999999999999e-12, tol_error));
        CHECK_THAT(differentiated_rbf(0.099),
            Catch::Matchers::WithinRel(-0.0195062534582674, tol_error));
        CHECK_THAT(differentiated_rbf(0.1),
            Catch::Matchers::WithinRel(-0.01990033250166389, tol_error));
        CHECK_THAT(differentiated_rbf(0.101),
            Catch::Matchers::WithinRel(-0.02029829253850752, tol_error));
        CHECK_THAT(differentiated_rbf(0.5),
            Catch::Matchers::WithinRel(-0.4423984338571903, tol_error));
        CHECK_THAT(differentiated_rbf(0.9),
            Catch::Matchers::WithinRel(-1.110283867554118, tol_error));
        CHECK_THAT(differentiated_rbf(1.0),
            Catch::Matchers::WithinRel(-1.264241117657115, tol_error));
        CHECK_THAT(differentiated_rbf(1.5),
            Catch::Matchers::WithinRel(-1.789201550876271, tol_error));
    }

    SECTION("calculate a second derivative") {
        // Same as gaussian_rbf from this order.
        constexpr double distance_rate = 1.2;
        constexpr double expected_value = 0.9477110347;
        const differentiated_t<
            differentiated_t<gaussian_from_square_rbf<double>>>
            differentiated_rbf;

        const double value = differentiated_rbf(distance_rate);

        constexpr double tol_error = 1e-4;
        CHECK_THAT(
            value, Catch::Matchers::WithinRel(expected_value, tol_error));
    }
}
