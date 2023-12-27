/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of build_first_coarse_grid_candidate function.
 */
#include "num_collect/linear/impl/amg/build_first_coarse_grid_candidate.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/linear/impl/amg/grid_type.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"

TEST_CASE("num_collect::linear::impl::amg::compute_node_scores") {
    using num_collect::linear::impl::amg::compute_node_scores;
    using num_collect::linear::impl::amg::node_connection_list;

    SECTION("compute scores") {
        /*
         * Grid:
         * 0          1
         * 2                 4
         *        3
         */
        node_connection_list<> connections;
        connections.push_back(2);
        connections.finish_current_node();
        connections.push_back(3);
        connections.finish_current_node();
        connections.push_back(0);
        connections.finish_current_node();
        connections.push_back(1);
        connections.finish_current_node();
        connections.push_back(1);
        connections.push_back(3);
        connections.finish_current_node();
        const auto transposed_connections = connections.transpose();

        const auto scores = compute_node_scores(transposed_connections);

        CHECK(scores == std::vector<int>{1, 2, 1, 2, 0});
    }
}

TEST_CASE("num_collect::linear::impl::amg::build_first_coarse_grid_candidate") {
    using num_collect::linear::impl::amg::build_first_coarse_grid_candidate;
    using num_collect::linear::impl::amg::grid_type;
    using num_collect::linear::impl::amg::node_connection_list;

    SECTION("build a candidate") {
        /*
         * Grid:
         * 0          1
         * 2                 4
         *        3
         */
        node_connection_list<> connections;
        connections.push_back(2);
        connections.finish_current_node();
        connections.push_back(3);
        connections.finish_current_node();
        connections.push_back(0);
        connections.finish_current_node();
        connections.push_back(1);
        connections.finish_current_node();
        connections.push_back(1);
        connections.push_back(3);
        connections.finish_current_node();
        const auto transposed_connections = connections.transpose();

        /*
         * First iteration:
         *   Score: [1, 2, 1, 2, 0]
         *   Coarse grid: [1]
         *   Fine grid: [3, 4]
         *   Remaining: [0, 2]
         * Second iteration:
         *   Score: [1, -, 1, -, -]
         *   Coarse grid: [0, 1]
         *   Fine grid: [2, 3, 4]
         */
        const auto candidate = build_first_coarse_grid_candidate(
            connections, transposed_connections);

        CHECK(candidate ==
            std::vector{grid_type::coarse, grid_type::coarse, grid_type::fine,
                grid_type::fine, grid_type::fine});
    }

    SECTION("build a candidate of another grid") {
        node_connection_list<> connections;
        connections.push_back(1);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(0);
        connections.finish_current_node();
        connections.push_back(0);
        connections.push_back(4);
        connections.finish_current_node();
        connections.push_back(1);
        connections.finish_current_node();
        const auto transposed_connections = connections.transpose();

        /*
         * First iteration:
         *   Score: [2, 2, 0, 0, 1]
         *   Coarse grid: [0]
         *   Fine grid: [2, 3]
         *   Remaining: [1, 4]
         * Second iteration:
         *   Score: [-, 1, -, -, 2]
         *   Coarse grid: [0, 4]
         *   Fine grid: [2, 3]
         *   Remaining: [1]
         */
        const auto candidate = build_first_coarse_grid_candidate(
            connections, transposed_connections);

        CHECK(candidate ==
            std::vector{grid_type::coarse, grid_type::coarse, grid_type::fine,
                grid_type::fine, grid_type::coarse});
    }

    SECTION("build a candidate of more another grid") {
        node_connection_list<> connections;
        connections.push_back(1);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(0);
        connections.finish_current_node();
        connections.push_back(0);
        connections.finish_current_node();
        connections.push_back(1);
        connections.finish_current_node();
        const auto transposed_connections = connections.transpose();

        /*
         * First iteration:
         *   Score: [2, 2, 0, 0, 0]
         *   Coarse grid: [0]
         *   Fine grid: [2, 3]
         *   Remaining: [1, 4]
         * Second iteration:
         *   Score: [-, 1, -, -, 0]
         *   Coarse grid: [0, 1]
         *   Fine grid: [2, 3, 4]
         */
        const auto candidate = build_first_coarse_grid_candidate(
            connections, transposed_connections);

        CHECK(candidate ==
            std::vector{grid_type::coarse, grid_type::coarse, grid_type::fine,
                grid_type::fine, grid_type::fine});
    }
}
