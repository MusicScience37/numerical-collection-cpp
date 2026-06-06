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
 * \brief Test of bessel_rbf class.
 */
#include "num_collect/rbf/rbfs/bessel_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/rbf.h"

TEST_CASE("num_collect::rbf::rbfs::bessel_rbf") {
    using num_collect::rbf::concepts::rbf;
    using num_collect::rbf::rbfs::bessel_rbf;

    SECTION("check of concepts") {
        STATIC_REQUIRE(rbf<bessel_rbf<double, 1>>);
        STATIC_REQUIRE(rbf<bessel_rbf<float, 2>>);
        STATIC_REQUIRE(rbf<bessel_rbf<long double, 3>>);
    }

    SECTION("positive definiteness") {
        CHECK(bessel_rbf<double, 1>::is_positive_definite == false);
        CHECK(bessel_rbf<double, 2>::is_positive_definite == true);
        CHECK(bessel_rbf<double, 3>::is_positive_definite == true);
    }

    constexpr double rel_tol = 1e-5;
    constexpr double abs_tol = 1e-5;

    SECTION("calculate values") {
        SECTION("Dimension = 1") {
            using rbf_type = bessel_rbf<double, 1>;
            const rbf_type rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.7978845608028654),
                    std::make_tuple(1e-6, 0.7978845608024664),
                    std::make_tuple(0.1, 0.7938984614098792),
                    std::make_tuple(0.4, 0.7349003466726426),
                    std::make_tuple(1.23, 0.2666831221104817),
                    std::make_tuple(4.56, -0.1211187634278972),
                    std::make_tuple(7.89, -0.02873228470920099),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Dimension = 2") {
            using rbf_type = bessel_rbf<double, 2>;
            const rbf_type rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 1.0),
                    std::make_tuple(1e-6, 0.99999999999975),
                    std::make_tuple(0.1, 0.99750156206604),
                    std::make_tuple(0.4, 0.9603982266595635),
                    std::make_tuple(1.23, 0.656070571706025),
                    std::make_tuple(4.56, -0.3062044901783967),
                    std::make_tuple(7.89, 0.19654526855521),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Dimension = 3") {
            using rbf_type = bessel_rbf<double, 3>;
            const rbf_type rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.7978845608028654),
                    std::make_tuple(1e-6, 0.7978845608027324),
                    std::make_tuple(0.1, 0.7965554179470395),
                    std::make_tuple(0.4, 0.7767772075537937),
                    std::make_tuple(1.23, 0.6113798892608872),
                    std::make_tuple(4.56, -0.1729469468522069),
                    std::make_tuple(7.89, 0.1010604638632208),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }

        SECTION("Dimension = 4") {
            using rbf_type = bessel_rbf<double, 4>;
            const rbf_type rbf;

            double distance_rate{};
            double expected_value{};
            std::tie(distance_rate, expected_value) =
                GENERATE(table<double, double>({
                    std::make_tuple(0.0, 0.5),
                    std::make_tuple(1e-6, 0.4999999999999375),
                    std::make_tuple(0.1, 0.49937526036242),
                    std::make_tuple(0.4, 0.4900664448882969),
                    std::make_tuple(1.23, 0.4112199777464196),
                    std::make_tuple(4.56, -0.05410144552611517),
                    std::make_tuple(7.89, 0.02756671837380112),
                }));
            INFO("distance_rate = " << distance_rate);

            const double actual_value = rbf(distance_rate);
            CHECK_THAT(actual_value,
                Catch::Matchers::WithinAbs(expected_value,
                    std::max(abs_tol, std::abs(expected_value) * rel_tol)));
        }
    }
}
