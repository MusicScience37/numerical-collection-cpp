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
 * \brief Definition of wendland_csrbf class.
 */
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

#include <tuple>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/csrbf.h"  // IWYU pragma: keep

TEMPLATE_TEST_CASE(
    "num_collect::rbf::rbfs::wendland_csrbf", "", float, double) {
    using num_collect::rbf::rbfs::wendland_csrbf;

    using scalar_type = TestType;

    SECTION("calculate values for l = 1, m = 0") {
        using rbf_type = wendland_csrbf<scalar_type, 1, 0>;
        STATIC_REQUIRE(num_collect::rbf::concepts::csrbf<rbf_type>);
        CHECK(rbf_type::support_boundary() ==
            static_cast<scalar_type>(1));  // NOLINT

        const rbf_type rbf;

        double distance_rate{};
        double expected_value{};
        std::tie(distance_rate, expected_value) =
            GENERATE(table<double, double>({
                {0.0, 1.0},  //NOLINT
                {0.7, 0.3},  // NOLINT
                {1.0, 0.0},  // NOLINT
                {1.2, 0.0}   // NOLINT
            }));

        constexpr auto tol_error = static_cast<scalar_type>(1e-4);
        CHECK_THAT(rbf(static_cast<scalar_type>(distance_rate)),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(expected_value), tol_error));
    }

    SECTION("calculate values for l = 2, m = 0") {
        using rbf_type = wendland_csrbf<scalar_type, 2, 0>;
        STATIC_REQUIRE(num_collect::rbf::concepts::csrbf<rbf_type>);
        CHECK(rbf_type::support_boundary() ==
            static_cast<scalar_type>(1));  // NOLINT

        const rbf_type rbf;

        double distance_rate{};
        double expected_value{};
        std::tie(distance_rate, expected_value) =
            GENERATE(table<double, double>({
                {0.0, 1.0},   //NOLINT
                {0.7, 0.09},  // NOLINT
                {1.0, 0.0},   // NOLINT
                {1.2, 0.0}    // NOLINT
            }));

        constexpr auto tol_error = static_cast<scalar_type>(1e-4);
        CHECK_THAT(rbf(static_cast<scalar_type>(distance_rate)),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(expected_value), tol_error));
    }

    SECTION("calculate values for l = 3, m = 0") {
        using rbf_type = wendland_csrbf<scalar_type, 3, 0>;
        STATIC_REQUIRE(num_collect::rbf::concepts::csrbf<rbf_type>);
        CHECK(rbf_type::support_boundary() ==
            static_cast<scalar_type>(1));  // NOLINT

        const rbf_type rbf;

        double distance_rate{};
        double expected_value{};
        std::tie(distance_rate, expected_value) =
            GENERATE(table<double, double>({
                {0.0, 1.0},    //NOLINT
                {0.7, 0.027},  // NOLINT
                {1.0, 0.0},    // NOLINT
                {1.2, 0.0}     // NOLINT
            }));

        constexpr auto tol_error = static_cast<scalar_type>(1e-4);
        CHECK_THAT(rbf(static_cast<scalar_type>(distance_rate)),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(expected_value), tol_error));
    }

    SECTION("calculate values for l = 1, m = 1") {
        using rbf_type = wendland_csrbf<scalar_type, 1, 1>;
        STATIC_REQUIRE(num_collect::rbf::concepts::csrbf<rbf_type>);
        CHECK(rbf_type::support_boundary() ==
            static_cast<scalar_type>(1));  // NOLINT

        const rbf_type rbf;

        double distance_rate{};
        double expected_value{};
        std::tie(distance_rate, expected_value) =
            GENERATE(table<double, double>({
                {0.0, 0.1666666666666},  //NOLINT
                {0.7, 0.036},            // NOLINT
                {1.0, 0.0},              // NOLINT
                {1.2, 0.0}               // NOLINT
            }));

        constexpr auto tol_error = static_cast<scalar_type>(1e-4);
        CHECK_THAT(rbf(static_cast<scalar_type>(distance_rate)),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(expected_value), tol_error));
    }

    SECTION("calculate values for l = 2, m = 1") {
        using rbf_type = wendland_csrbf<scalar_type, 2, 1>;
        STATIC_REQUIRE(num_collect::rbf::concepts::csrbf<rbf_type>);
        CHECK(rbf_type::support_boundary() ==
            static_cast<scalar_type>(1));  // NOLINT

        const rbf_type rbf;

        double distance_rate{};
        double expected_value{};
        std::tie(distance_rate, expected_value) =
            GENERATE(table<double, double>({
                {0.0, 0.08333333333333333},  //NOLINT
                {0.7, 0.006975},             // NOLINT
                {1.0, 0.0},                  // NOLINT
                {1.2, 0.0}                   // NOLINT
            }));

        constexpr auto tol_error = static_cast<scalar_type>(1e-4);
        CHECK_THAT(rbf(static_cast<scalar_type>(distance_rate)),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(expected_value), tol_error));
    }

    SECTION("calculate values for l = 1, m = 2") {
        using rbf_type = wendland_csrbf<scalar_type, 1, 2>;
        STATIC_REQUIRE(num_collect::rbf::concepts::csrbf<rbf_type>);
        CHECK(rbf_type::support_boundary() ==
            static_cast<scalar_type>(1));  // NOLINT

        const rbf_type rbf;

        double distance_rate{};
        double expected_value{};
        std::tie(distance_rate, expected_value) =
            GENERATE(table<double, double>({
                {0.0, 0.025},      //NOLINT
                {0.7, 0.0029745},  // NOLINT
                {1.0, 0.0},        // NOLINT
                {1.2, 0.0}         // NOLINT
            }));

        constexpr auto tol_error = static_cast<scalar_type>(1e-4);
        CHECK_THAT(rbf(static_cast<scalar_type>(distance_rate)),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(expected_value), tol_error));
    }

    SECTION("calculate values for l = 2, m = 2") {
        using rbf_type = wendland_csrbf<scalar_type, 2, 2>;
        STATIC_REQUIRE(num_collect::rbf::concepts::csrbf<rbf_type>);
        CHECK(rbf_type::support_boundary() ==
            static_cast<scalar_type>(1));  // NOLINT

        const rbf_type rbf;

        double distance_rate{};
        double expected_value{};
        std::tie(distance_rate, expected_value) =
            GENERATE(table<double, double>({
                {0.0, 0.008333333333333333},  //NOLINT
                {0.7, 0.000421875},           // NOLINT
                {1.0, 0.0},                   // NOLINT
                {1.2, 0.0}                    // NOLINT
            }));

        constexpr auto tol_error = static_cast<scalar_type>(1e-4);
        CHECK_THAT(rbf(static_cast<scalar_type>(distance_rate)),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(expected_value), tol_error));
    }
}
