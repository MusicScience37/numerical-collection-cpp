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
 * \brief Test of generate_rectangle_boundary_nodes function.
 */
#include "num_collect/util/generate_rectangle_boundary_nodes.h"

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/vector.h"

TEMPLATE_TEST_CASE("num_collect::util::generate_rectangle_boundary_nodes", "",
    float, double, long double) {
    using num_collect::util::generate_rectangle_boundary_nodes;

    using scalar_type = TestType;
    using node_type = Eigen::Vector2<scalar_type>;

    const node_type lower_left{
        static_cast<scalar_type>(0), static_cast<scalar_type>(0)};
    const node_type upper_right{
        static_cast<scalar_type>(2), static_cast<scalar_type>(3)};

    SECTION("counter-clockwise (default) with num_nodes_per_edge = 4") {
        constexpr num_collect::index_type num_nodes_per_edge = 4;

        num_collect::util::vector<node_type> nodes;
        REQUIRE_NOTHROW(nodes = generate_rectangle_boundary_nodes<scalar_type>(
                            lower_left, upper_right, num_nodes_per_edge));

        REQUIRE(
            nodes.size() == static_cast<std::size_t>(4 * num_nodes_per_edge));

        // dx = 2/4 = 0.5, dy = 3/4 = 0.75
        constexpr auto approx_eps =
            static_cast<scalar_type>(1e-5);  // tolerance for float

        // Bottom edge: (0,0), (0.5,0), (1,0), (1.5,0)
        CHECK_THAT(nodes[0].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[0].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[1].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0.5), approx_eps));
        CHECK_THAT(nodes[1].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[3].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(1.5), approx_eps));
        CHECK_THAT(nodes[3].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));

        // Right edge: (2,0), (2,0.75), (2,1.5), (2,2.25)
        CHECK_THAT(nodes[4].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[4].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[5].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[5].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0.75), approx_eps));

        // Top edge: (2,3), (1.5,3), (1,3), (0.5,3)
        CHECK_THAT(nodes[8].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[8].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));
        CHECK_THAT(nodes[9].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(1.5), approx_eps));
        CHECK_THAT(nodes[9].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));

        // Left edge: (0,3), (0,2.25), (0,1.5), (0,0.75)
        CHECK_THAT(nodes[12].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[12].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));
        CHECK_THAT(nodes[15].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[15].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0.75), approx_eps));
    }

    SECTION("clockwise with num_nodes_per_edge = 4") {
        constexpr num_collect::index_type num_nodes_per_edge = 4;

        num_collect::util::vector<node_type> nodes;
        REQUIRE_NOTHROW(nodes = generate_rectangle_boundary_nodes<scalar_type>(
                            lower_left, upper_right, num_nodes_per_edge, true));

        REQUIRE(
            nodes.size() == static_cast<std::size_t>(4 * num_nodes_per_edge));

        constexpr auto approx_eps = static_cast<scalar_type>(1e-5);

        // Left-up edge: (0,0), (0,0.75), (0,1.5), (0,2.25)
        CHECK_THAT(nodes[0].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[0].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[1].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[1].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0.75), approx_eps));

        // Top-right edge: (0,3), (0.5,3), (1,3), (1.5,3)
        CHECK_THAT(nodes[4].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[4].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));
        CHECK_THAT(nodes[5].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0.5), approx_eps));
        CHECK_THAT(nodes[5].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));

        // Right-down edge: (2,3), (2,2.25), (2,1.5), (2,0.75)
        CHECK_THAT(nodes[8].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[8].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));
        CHECK_THAT(nodes[9].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[9].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2.25), approx_eps));

        // Bottom-left edge: (2,0), (1.5,0), (1,0), (0.5,0)
        CHECK_THAT(nodes[12].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[12].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[15].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0.5), approx_eps));
        CHECK_THAT(nodes[15].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
    }

    SECTION("num_nodes_per_edge = 1") {
        num_collect::util::vector<node_type> nodes;
        REQUIRE_NOTHROW(nodes = generate_rectangle_boundary_nodes<scalar_type>(
                            lower_left, upper_right, 1));

        REQUIRE(nodes.size() == 4U);

        constexpr auto approx_eps = static_cast<scalar_type>(1e-5);

        // Bottom-left corner
        CHECK_THAT(nodes[0].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[0].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        // Bottom-right corner
        CHECK_THAT(nodes[1].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[1].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        // Upper-right corner
        CHECK_THAT(nodes[2].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(2), approx_eps));
        CHECK_THAT(nodes[2].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));
        // Upper-left corner
        CHECK_THAT(nodes[3].x(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(0), approx_eps));
        CHECK_THAT(nodes[3].y(),
            Catch::Matchers::WithinAbs(
                static_cast<scalar_type>(3), approx_eps));
    }

    SECTION("invalid num_nodes_per_edge = 0") {
        REQUIRE_THROWS_AS(generate_rectangle_boundary_nodes<scalar_type>(
                              lower_left, upper_right, 0),
            num_collect::precondition_not_satisfied);
    }
}
