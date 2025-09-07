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
 * \brief Test of thin_plate_spline_rbf class.
 */
#include "num_collect/rbf/rbfs/thin_plate_spline_rbf.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/constants/pi.h"
#include "num_collect/rbf/concepts/rbf.h"

TEST_CASE("num_collect::rbf::rbfs::thin_plate_spline_rbf") {
    using num_collect::rbf::rbfs::thin_plate_spline_rbf;

    constexpr double rel_tol = 1e-5;

    SECTION("Dimension = 1") {
        SECTION("Degree = 1") {
            using rbf_type = thin_plate_spline_rbf<double, 1, 1>;
            STATIC_REQUIRE(num_collect::rbf::concepts::rbf<rbf_type>);

            SECTION("calculate coefficient") {
                constexpr double actual = rbf_type::coefficient();
                constexpr double expected = -0.5;
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-10, rel_tol));
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(
                    rbf(1.5), Catch::Matchers::WithinRel(coeff * 1.5, rel_tol));
            }
        }

        SECTION("Degree = 2") {
            using rbf_type = thin_plate_spline_rbf<double, 1, 2>;
            STATIC_REQUIRE(num_collect::rbf::concepts::rbf<rbf_type>);

            SECTION("calculate coefficient") {
                constexpr double actual = rbf_type::coefficient();
                constexpr double expected = 1.0 / 12.0;
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-30, rel_tol));
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(rbf(1.5),
                    Catch::Matchers::WithinRel(coeff * 3.375, rel_tol));
            }
        }
    }

    SECTION("Dimension = 2") {
        SECTION("Degree = 2") {
            using rbf_type = thin_plate_spline_rbf<double, 2, 2>;
            STATIC_REQUIRE(num_collect::rbf::concepts::rbf<rbf_type>);

            SECTION("calculate coefficient") {
                constexpr double actual = rbf_type::coefficient();
                constexpr double expected =
                    1.0 / (8.0 * num_collect::constants::pi<double>);
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(
                        coeff * -2.302585093e-19, rel_tol));
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(rbf(1.5),
                    Catch::Matchers::WithinRel(coeff * 0.9122964932, rel_tol));
            }
        }
    }

    SECTION("Dimension = 3") {
        SECTION("Degree = 2") {
            using rbf_type = thin_plate_spline_rbf<double, 3, 2>;
            STATIC_REQUIRE(num_collect::rbf::concepts::rbf<rbf_type>);

            SECTION("calculate coefficient") {
                constexpr double actual = rbf_type::coefficient();
                constexpr double expected =
                    -1.0 / (8.0 * num_collect::constants::pi<double>);
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-10, rel_tol));
                // NOLINTNEXTLINE(*-magic-numbers)
                CHECK_THAT(
                    rbf(1.5), Catch::Matchers::WithinRel(coeff * 1.5, rel_tol));
            }
        }
    }
}
