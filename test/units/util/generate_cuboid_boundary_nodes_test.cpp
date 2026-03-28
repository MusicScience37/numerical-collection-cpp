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
 * \brief Test of generate_cuboid_boundary_nodes function.
 */
#include "num_collect/util/generate_cuboid_boundary_nodes.h"

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/util/vector.h"

TEMPLATE_TEST_CASE("num_collect::util::generate_cuboid_boundary_nodes", "",
    float, double, long double) {
    using num_collect::util::generate_cuboid_boundary_nodes;

    using scalar_type = TestType;
    using node_type = Eigen::Vector3<scalar_type>;

    const node_type lower{static_cast<scalar_type>(0),
        static_cast<scalar_type>(0), static_cast<scalar_type>(0)};
    const node_type upper{static_cast<scalar_type>(2),
        static_cast<scalar_type>(2), static_cast<scalar_type>(2)};

    SECTION("num_nodes_per_edge = 2") {
        num_collect::util::vector<node_type> nodes;

        REQUIRE_NOTHROW(
            nodes = generate_cuboid_boundary_nodes(lower, upper, 2));

        REQUIRE(nodes.size() == 26U);
        CHECK_THAT(nodes[0], eigen_approx(node_type{0.0, 0.0, 0.0}));
        CHECK_THAT(nodes[1], eigen_approx(node_type{0.0, 0.0, 2.0}));
        CHECK_THAT(nodes[2], eigen_approx(node_type{0.0, 1.0, 0.0}));
        CHECK_THAT(nodes[3], eigen_approx(node_type{0.0, 1.0, 2.0}));
        CHECK_THAT(nodes[4], eigen_approx(node_type{0.0, 2.0, 0.0}));
        CHECK_THAT(nodes[5], eigen_approx(node_type{0.0, 2.0, 2.0}));
        CHECK_THAT(nodes[6], eigen_approx(node_type{1.0, 0.0, 0.0}));
        CHECK_THAT(nodes[7], eigen_approx(node_type{1.0, 0.0, 2.0}));
        CHECK_THAT(nodes[8], eigen_approx(node_type{1.0, 1.0, 0.0}));
        CHECK_THAT(nodes[9], eigen_approx(node_type{1.0, 1.0, 2.0}));
        CHECK_THAT(nodes[10], eigen_approx(node_type{1.0, 2.0, 0.0}));
        CHECK_THAT(nodes[11], eigen_approx(node_type{1.0, 2.0, 2.0}));
        CHECK_THAT(nodes[12], eigen_approx(node_type{2.0, 0.0, 0.0}));
        CHECK_THAT(nodes[13], eigen_approx(node_type{2.0, 0.0, 2.0}));
        CHECK_THAT(nodes[14], eigen_approx(node_type{2.0, 1.0, 0.0}));
        CHECK_THAT(nodes[15], eigen_approx(node_type{2.0, 1.0, 2.0}));
        CHECK_THAT(nodes[16], eigen_approx(node_type{2.0, 2.0, 0.0}));
        CHECK_THAT(nodes[17], eigen_approx(node_type{2.0, 2.0, 2.0}));
        CHECK_THAT(nodes[18], eigen_approx(node_type{0.0, 0.0, 1.0}));
        CHECK_THAT(nodes[19], eigen_approx(node_type{0.0, 2.0, 1.0}));
        CHECK_THAT(nodes[20], eigen_approx(node_type{1.0, 0.0, 1.0}));
        CHECK_THAT(nodes[21], eigen_approx(node_type{1.0, 2.0, 1.0}));
        CHECK_THAT(nodes[22], eigen_approx(node_type{2.0, 0.0, 1.0}));
        CHECK_THAT(nodes[23], eigen_approx(node_type{2.0, 2.0, 1.0}));
        CHECK_THAT(nodes[24], eigen_approx(node_type{0.0, 1.0, 1.0}));
        CHECK_THAT(nodes[25], eigen_approx(node_type{2.0, 1.0, 1.0}));
    }

    SECTION("num_nodes_per_edge = 1") {
        num_collect::util::vector<node_type> nodes;

        REQUIRE_NOTHROW(
            nodes = generate_cuboid_boundary_nodes(lower, upper, 1));

        REQUIRE(nodes.size() == 8U);
        CHECK_THAT(nodes[0], eigen_approx(node_type{0.0, 0.0, 0.0}));
        CHECK_THAT(nodes[1], eigen_approx(node_type{0.0, 0.0, 2.0}));
        CHECK_THAT(nodes[2], eigen_approx(node_type{0.0, 2.0, 0.0}));
        CHECK_THAT(nodes[3], eigen_approx(node_type{0.0, 2.0, 2.0}));
        CHECK_THAT(nodes[4], eigen_approx(node_type{2.0, 0.0, 0.0}));
        CHECK_THAT(nodes[5], eigen_approx(node_type{2.0, 0.0, 2.0}));
        CHECK_THAT(nodes[6], eigen_approx(node_type{2.0, 2.0, 0.0}));
        CHECK_THAT(nodes[7], eigen_approx(node_type{2.0, 2.0, 2.0}));
    }

    SECTION("num_nodes_per_edge = 10") {
        num_collect::util::vector<node_type> nodes;

        REQUIRE_NOTHROW(
            nodes = generate_cuboid_boundary_nodes(lower, upper, 10));
        // Only check that no error happens.
    }

    SECTION("num_nodes_per_edge = 0 (error)") {
        CHECK_THROWS(generate_cuboid_boundary_nodes(lower, upper, 0));
    }
}
