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

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "is_finite.h"
#include "num_collect/base/constants.h"
#include "num_collect/functions/sqrt.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/rbfs/differentiated.h"

TEST_CASE("num_collect::rbf::rbfs::thin_plate_spline_rbf") {
    using num_collect::rbf::rbfs::differentiated_t;
    using num_collect::rbf::rbfs::thin_plate_spline_rbf;

    constexpr double rel_tol = 1e-5;
    constexpr double second_diff_rel_tol =
        0.2;  // Numeric second derivative is unstable.
    static constexpr double diff_width =
        num_collect::functions::sqrt(std::numeric_limits<double>::epsilon());
    const auto diff = [](const auto& rbf, const double distance) {
        return (rbf(distance + diff_width) - rbf(distance - diff_width)) /
            (2.0 * diff_width) / (-distance);
    };
    const auto second_diff = [&diff](const auto& rbf, const double distance) {
        return (diff(rbf, distance + diff_width) -
                   diff(rbf, distance - diff_width)) /
            (2.0 * diff_width) / (-distance);
    };

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
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-10, rel_tol));
                CHECK_THAT(
                    rbf(1.5), Catch::Matchers::WithinRel(coeff * 1.5, rel_tol));
            }

            // In this case, the differentiated RBF is not defined.
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
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-30, rel_tol));
                CHECK_THAT(rbf(1.5),
                    Catch::Matchers::WithinRel(coeff * 3.375, rel_tol));
            }

            SECTION("calculate derivatives") {
                differentiated_t<rbf_type> differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(differentiated_rbf(0.0), is_finite());
                CHECK_THAT(differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(diff(rbf, 0.1), rel_tol));
                CHECK_THAT(differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(diff(rbf, 1.5), rel_tol));
            }

            SECTION("calculate second derivatives") {
                differentiated_t<differentiated_t<rbf_type>>
                    second_differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(second_differentiated_rbf(0.0), is_finite());
                CHECK_THAT(second_differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 0.1), second_diff_rel_tol));
                CHECK_THAT(second_differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 1.5), second_diff_rel_tol));
            }
        }

        SECTION("Degree = 3") {
            using rbf_type = thin_plate_spline_rbf<double, 1, 3>;
            STATIC_REQUIRE(num_collect::rbf::concepts::rbf<rbf_type>);

            SECTION("calculate coefficient") {
                constexpr double actual = rbf_type::coefficient();
                constexpr double expected = -0.004166666666666666;
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-50, rel_tol));
                CHECK_THAT(rbf(1.5),
                    Catch::Matchers::WithinRel(coeff * 7.59375, rel_tol));
            }

            SECTION("calculate derivatives") {
                differentiated_t<rbf_type> differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(differentiated_rbf(0.0), is_finite());
                CHECK_THAT(differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(diff(rbf, 0.1), rel_tol));
                CHECK_THAT(differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(diff(rbf, 1.5), rel_tol));
            }

            SECTION("calculate second derivatives") {
                differentiated_t<differentiated_t<rbf_type>>
                    second_differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(second_differentiated_rbf(0.0), is_finite());
                CHECK_THAT(second_differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 0.1), second_diff_rel_tol));
                CHECK_THAT(second_differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 1.5), second_diff_rel_tol));
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
                    1.0 / (8.0 * num_collect::pi<double>);
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(
                        coeff * -2.302585093e-19, rel_tol));
                CHECK_THAT(rbf(1.5),
                    Catch::Matchers::WithinRel(coeff * 0.9122964932, rel_tol));
            }

            SECTION("calculate derivatives") {
                differentiated_t<rbf_type> differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(differentiated_rbf(0.0), is_finite());
                CHECK_THAT(differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(diff(rbf, 0.1), rel_tol));
                CHECK_THAT(differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(diff(rbf, 1.5), rel_tol));
            }

            // In this case, the twice-differentiated RBF is not defined.
        }

        SECTION("Degree = 3") {
            using rbf_type = thin_plate_spline_rbf<double, 2, 3>;
            STATIC_REQUIRE(num_collect::rbf::concepts::rbf<rbf_type>);

            SECTION("calculate coefficient") {
                constexpr double actual = rbf_type::coefficient();
                constexpr double expected = -0.0024867959858108648;
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(
                        coeff * -2.302585092994046e-39, rel_tol));
                CHECK_THAT(rbf(1.5),
                    Catch::Matchers::WithinRel(
                        coeff * 2.052667109797582, rel_tol));
            }

            SECTION("calculate derivatives") {
                differentiated_t<rbf_type> differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(differentiated_rbf(0.0), is_finite());
                CHECK_THAT(differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(diff(rbf, 0.1), rel_tol));
                CHECK_THAT(differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(diff(rbf, 1.5), rel_tol));
            }

            SECTION("calculate second derivatives") {
                differentiated_t<differentiated_t<rbf_type>>
                    second_differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(second_differentiated_rbf(0.0), is_finite());
                CHECK_THAT(second_differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 0.1), second_diff_rel_tol));
                CHECK_THAT(second_differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 1.5), second_diff_rel_tol));
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
                    -1.0 / (8.0 * num_collect::pi<double>);
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-10, rel_tol));
                CHECK_THAT(
                    rbf(1.5), Catch::Matchers::WithinRel(coeff * 1.5, rel_tol));
            }

            // In this case, the differentiated RBF is not defined.
        }

        SECTION("Degree = 3") {
            using rbf_type = thin_plate_spline_rbf<double, 3, 3>;
            STATIC_REQUIRE(num_collect::rbf::concepts::rbf<rbf_type>);

            SECTION("calculate coefficient") {
                constexpr double actual = rbf_type::coefficient();
                constexpr double expected = 0.003315727981081152;
                CHECK_THAT(
                    actual, Catch::Matchers::WithinRel(expected, rel_tol));
            }

            SECTION("calculate values") {
                rbf_type rbf;
                constexpr double coeff = rbf_type::coefficient();

                CHECK(rbf(0.0) == 0.0);
                CHECK_THAT(rbf(1e-10),
                    Catch::Matchers::WithinRel(coeff * 1e-30, rel_tol));
                CHECK_THAT(rbf(1.5),
                    Catch::Matchers::WithinRel(coeff * 3.375, rel_tol));
            }

            SECTION("calculate derivatives") {
                differentiated_t<rbf_type> differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(differentiated_rbf(0.0), is_finite());
                CHECK_THAT(differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(diff(rbf, 0.1), rel_tol));
                CHECK_THAT(differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(diff(rbf, 1.5), rel_tol));
            }

            SECTION("calculate second derivatives") {
                differentiated_t<differentiated_t<rbf_type>>
                    second_differentiated_rbf;
                rbf_type rbf;

                CHECK_THAT(second_differentiated_rbf(0.0), is_finite());
                CHECK_THAT(second_differentiated_rbf(0.1),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 0.1), second_diff_rel_tol));
                CHECK_THAT(second_differentiated_rbf(1.5),
                    Catch::Matchers::WithinRel(
                        second_diff(rbf, 1.5), second_diff_rel_tol));
            }
        }
    }
}
