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
 * \brief Test of matern_rbf class.
 */
#include "num_collect/rbf/rbfs/matern_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/rbf.h"

TEST_CASE("num_collect::rbf::rbfs::matern_rbf") {
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::matern_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<matern_rbf<double, 0.5>>);
        STATIC_REQUIRE(rbf<matern_rbf<float, 1.3F>>);
        STATIC_REQUIRE(rbf<matern_rbf<double, 2.3>>);
    }

    SECTION("positive definiteness") {
        CHECK(matern_rbf<double, 0.5>::is_positive_definite);
        CHECK(matern_rbf<float, 1.3F>::is_positive_definite);
        CHECK(matern_rbf<double, 2.3>::is_positive_definite);
    }

    constexpr double rel_tol = 1e-5;
    constexpr double abs_tol = 1e-5;

    SECTION("calculate values") {
        SECTION("Order = 0.5") {
            using rbf_type = matern_rbf<double, 0.5>;
            const rbf_type rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 1.0),
                    std::make_tuple(1e-6, 0.9999990000005),
                    std::make_tuple(1e-5, 0.9999900000499998),
                    std::make_tuple(0.1, 0.9048374180359596),
                    std::make_tuple(0.4, 0.6703200460356393),
                    std::make_tuple(1.23, 0.2922925776808594),
                    std::make_tuple(4.56, 0.0104620589434268),
                    std::make_tuple(7.89, 0.0003744695749860782),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Order = 1.0") {
            using rbf_type = matern_rbf<double, 1.0>;
            const rbf_type rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 1.0),
                    std::make_tuple(1e-6, 0.9999999999927843),
                    std::make_tuple(1e-5, 0.9999999993935572),
                    std::make_tuple(0.1, 0.9853844780870606),
                    std::make_tuple(0.4, 0.873741769893075),
                    std::make_tuple(1.23, 0.5101361444056312),
                    std::make_tuple(4.56, 0.03016850585278583),
                    std::make_tuple(7.89, 0.001378712899008262),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Order = 1.5") {
            using rbf_type = matern_rbf<double, 1.5>;
            const rbf_type rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 1.0),
                    std::make_tuple(1e-6, 0.9999999999995),
                    std::make_tuple(1e-5, 0.9999999999500003),
                    std::make_tuple(0.1, 0.9953211598395555),
                    std::make_tuple(0.4, 0.938448064449895),
                    std::make_tuple(1.23, 0.6518124482283165),
                    std::make_tuple(4.56, 0.058169047725453),
                    std::make_tuple(7.89, 0.003329034521626235),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }
    }
}
