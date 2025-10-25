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

#include <string>
#include <string_view>
#include <vector>

#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <fmt/format.h>

#include "fmt_approval_tests.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/impl/amg/compute_strong_connection_list.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"
#include "num_collect/linear/impl/amg/node_layer.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

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

        const auto table = compute_node_scores(transposed_connections);
        std::vector<int> scores(5);
        for (const auto& [score, index] : table.score_to_index_map()) {
            scores.at(static_cast<std::size_t>(index)) = score;
        }

        CHECK_THAT(scores,
            Catch::Matchers::RangeEquals(std::vector<int>{1, 2, 1, 2, 0}));
    }
}

TEST_CASE("num_collect::linear::impl::amg::build_first_coarse_grid_candidate") {
    using num_collect::linear::impl::amg::build_first_coarse_grid_candidate;
    using num_collect::linear::impl::amg::node_connection_list;
    using num_collect::linear::impl::amg::node_layer;

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

        CHECK_THAT(candidate,
            Catch::Matchers::RangeEquals(
                std::vector{node_layer::coarse, node_layer::coarse,
                    node_layer::fine, node_layer::fine, node_layer::fine}));
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

        CHECK_THAT(candidate,
            Catch::Matchers::RangeEquals(
                std::vector{node_layer::coarse, node_layer::coarse,
                    node_layer::fine, node_layer::fine, node_layer::coarse}));
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

        CHECK_THAT(candidate,
            Catch::Matchers::RangeEquals(
                std::vector{node_layer::coarse, node_layer::coarse,
                    node_layer::fine, node_layer::fine, node_layer::fine}));
    }

    SECTION("build a candidate for laplacian_2d_grid") {
        using num_prob_collect::linear::laplacian_2d_grid;
        using scalar_type = double;
        using matrix_type = Eigen::SparseMatrix<scalar_type>;
        using storage_index_type = typename matrix_type::StorageIndex;
        using num_collect::linear::impl::amg::compute_strong_connection_list;

        constexpr num_collect::index_type grid_size = 10;
        constexpr auto grid_width = static_cast<scalar_type>(0.1);
        constexpr auto strong_coeff_rate_threshold =
            static_cast<scalar_type>(0.5);
        laplacian_2d_grid<matrix_type> grid{grid_size, grid_size, grid_width};
        const matrix_type& matrix = grid.mat();
        const auto connections =
            compute_strong_connection_list(matrix, strong_coeff_rate_threshold);
        const auto transposed_connections = connections.transpose();

        const auto candidate = build_first_coarse_grid_candidate(
            connections, transposed_connections);
        fmt::memory_buffer buffer;
        buffer.append(std::string_view("Classification:"));
        for (num_collect::index_type i = 0; i < candidate.size(); ++i) {
            if (i % grid_size == 0U) {
                buffer.push_back('\n');
            }
            if (candidate[i] == node_layer::coarse) {
                buffer.push_back('x');
            } else {
                buffer.push_back('o');
            }
            buffer.push_back(' ');
        }
        ApprovalTests::Approvals::verify(
            std::string(buffer.data(), buffer.size()));
    }
}
