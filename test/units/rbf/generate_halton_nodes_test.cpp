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
 * \brief Test of generate_halton_nodes function.
 */
#include "num_collect/rbf/generate_halton_nodes.h"

#include <algorithm>
#include <vector>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::rbf::impl::generate_halton_nodes_impl") {
    using num_collect::rbf::impl::generate_halton_nodes_impl;

    SECTION("generate a sequence") {
        const num_collect::index_type base_sequence_size =
            GENERATE(2, 3, 5, 7, 11, 13);
        const num_collect::index_type num_nodes =
            GENERATE(Catch::Generators::range(10, 50));

        Eigen::VectorXd buffer;
        generate_halton_nodes_impl(buffer, num_nodes, base_sequence_size);

        REQUIRE(buffer.size() == num_nodes + 1);
        for (num_collect::index_type i = 1; i <= num_nodes; ++i) {
            INFO("i = " << i);
            CHECK(buffer(i) > 0.0);
            CHECK(buffer(i) < 1.0);
        }
        std::sort(buffer.begin(), buffer.end());
        for (num_collect::index_type i = 1; i <= num_nodes; ++i) {
            INFO("i = " << i);
            CHECK(buffer(i) > buffer(i - 1));
        }
    }
}

TEMPLATE_TEST_CASE(
    "num_collect::rbf::generate_halton_nodes", "", float, double, long double) {
    using num_collect::rbf::generate_halton_nodes;

    using scalar_type = TestType;

    SECTION("generate 2D nodes") {
        constexpr num_collect::index_type dimensions = 2;
        constexpr num_collect::index_type num_nodes = 127;
        using node_type = Eigen::Vector<scalar_type, dimensions>;

        std::vector<node_type> nodes;
        REQUIRE_NOTHROW(
            nodes = generate_halton_nodes<scalar_type, dimensions>(num_nodes));

        REQUIRE(nodes.size() == static_cast<std::size_t>(num_nodes));
        for (std::size_t i = 0; i < nodes.size(); ++i) {
            INFO("i = " << i);
            for (num_collect::index_type d = 0; d < dimensions; ++d) {
                INFO("d = " << d);
                CHECK(nodes[i](d) > static_cast<scalar_type>(0));
                CHECK(nodes[i](d) < static_cast<scalar_type>(1));
            }
        }
    }

    SECTION("generate 3D nodes") {
        constexpr num_collect::index_type dimensions = 3;
        constexpr num_collect::index_type num_nodes = 127;
        using node_type = Eigen::Vector<scalar_type, dimensions>;

        std::vector<node_type> nodes;
        REQUIRE_NOTHROW(
            nodes = generate_halton_nodes<scalar_type, dimensions>(num_nodes));

        REQUIRE(nodes.size() == static_cast<std::size_t>(num_nodes));
        for (std::size_t i = 0; i < nodes.size(); ++i) {
            INFO("i = " << i);
            for (num_collect::index_type d = 0; d < dimensions; ++d) {
                INFO("d = " << d);
                CHECK(nodes[i](d) > static_cast<scalar_type>(0));
                CHECK(nodes[i](d) < static_cast<scalar_type>(1));
            }
        }
    }

    SECTION("generate 6D nodes") {
        constexpr num_collect::index_type dimensions = 6;
        constexpr num_collect::index_type num_nodes = 127;
        using node_type = Eigen::Vector<scalar_type, dimensions>;

        std::vector<node_type> nodes;
        REQUIRE_NOTHROW(
            nodes = generate_halton_nodes<scalar_type, dimensions>(num_nodes));

        REQUIRE(nodes.size() == static_cast<std::size_t>(num_nodes));
        for (std::size_t i = 0; i < nodes.size(); ++i) {
            INFO("i = " << i);
            for (num_collect::index_type d = 0; d < dimensions; ++d) {
                INFO("d = " << d);
                CHECK(nodes[i](d) > static_cast<scalar_type>(0));
                CHECK(nodes[i](d) < static_cast<scalar_type>(1));
            }
        }
    }
}

TEMPLATE_TEST_CASE("num_collect::rbf::generate_1d_halton_nodes", "", float,
    double, long double) {
    using num_collect::rbf::generate_1d_halton_nodes;

    using scalar_type = TestType;

    SECTION("generate 1D nodes") {
        constexpr num_collect::index_type num_nodes = 331;

        std::vector<scalar_type> nodes;
        REQUIRE_NOTHROW(
            nodes = generate_1d_halton_nodes<scalar_type>(num_nodes));

        REQUIRE(nodes.size() == static_cast<std::size_t>(num_nodes));
        for (std::size_t i = 0; i < nodes.size(); ++i) {
            INFO("i = " << i);
            CHECK(nodes[i] > static_cast<scalar_type>(0));
            CHECK(nodes[i] < static_cast<scalar_type>(1));
        }
    }
}
