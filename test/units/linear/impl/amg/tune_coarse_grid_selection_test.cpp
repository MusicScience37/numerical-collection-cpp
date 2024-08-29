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
 * \brief Test of tune_coarse_grid_selection function.
 */
#include "num_collect/linear/impl/amg/tune_coarse_grid_selection.h"

#include <string>
#include <string_view>
#include <unordered_set>

#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <fmt/format.h>

#include "fmt_approval_tests.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/impl/amg/build_first_coarse_grid_candidate.h"
#include "num_collect/linear/impl/amg/compute_strong_connection_list.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"
#include "num_collect/linear/impl/amg/node_layer.h"
#include "num_collect/util/vector.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

TEST_CASE(
    "num_collect::linear::impl::amg::find_node_unsatisfying_interpolation_"
    "condition") {
    using num_collect::linear::impl::amg::
        find_node_unsatisfying_interpolation_condition;
    using num_collect::linear::impl::amg::node_connection_list;

    SECTION("find a node") {
        node_connection_list<int> connections;
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(1);
        connections.push_back(4);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(0);
        connections.push_back(2);
        connections.finish_current_node();
        const auto neighbors_in_coarse_grid = std::unordered_set<int>{1, 3};
        const auto neighbors_in_fine_grid = std::unordered_set<int>{2, 4};

        const auto node = find_node_unsatisfying_interpolation_condition(
            connections, neighbors_in_coarse_grid, neighbors_in_fine_grid);

        CHECK(node == 4);
    }

    SECTION("find no node") {
        node_connection_list<int> connections;
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(1);
        connections.push_back(4);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(0);
        connections.push_back(1);
        connections.push_back(2);
        connections.finish_current_node();
        const auto neighbors_in_coarse_grid = std::unordered_set<int>{1, 3};
        const auto neighbors_in_fine_grid = std::unordered_set<int>{2, 4};

        const auto node = find_node_unsatisfying_interpolation_condition(
            connections, neighbors_in_coarse_grid, neighbors_in_fine_grid);

        CHECK(node == std::nullopt);
    }
}

TEST_CASE(
    "num_collect::linear::impl::amg::tune_coarse_grid_selection_for_one_node") {
    using num_collect::linear::impl::amg::node_connection_list;
    using num_collect::linear::impl::amg::node_layer;
    using num_collect::linear::impl::amg::
        tune_coarse_grid_selection_for_one_node;
    using num_collect::util::vector;

    SECTION("change no nodes when all neighbors are OK") {
        node_connection_list<int> connections;
        connections.push_back(1);
        connections.push_back(2);
        connections.push_back(3);
        connections.push_back(4);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(1);
        connections.push_back(4);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(0);
        connections.push_back(1);
        connections.push_back(2);
        connections.finish_current_node();
        vector<node_layer> node_classification{node_layer::fine,
            node_layer::coarse, node_layer::fine, node_layer::coarse,
            node_layer::fine};
        constexpr num_collect::index_type tested_node_index = 0;

        tune_coarse_grid_selection_for_one_node(
            connections, node_classification, tested_node_index);

        CHECK_THAT(node_classification,
            Catch::Matchers::RangeEquals(
                vector<node_layer>{node_layer::fine, node_layer::coarse,
                    node_layer::fine, node_layer::coarse, node_layer::fine}));
    }

    SECTION(
        "change a neighboring node if the only one neighboring node is not "
        "OK") {
        node_connection_list<int> connections;
        connections.push_back(1);
        connections.push_back(2);
        connections.push_back(3);
        connections.push_back(4);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(1);
        connections.push_back(4);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(0);
        connections.push_back(2);
        connections.finish_current_node();
        vector<node_layer> node_classification{node_layer::fine,
            node_layer::coarse, node_layer::fine, node_layer::coarse,
            node_layer::fine};
        constexpr num_collect::index_type tested_node_index = 0;

        tune_coarse_grid_selection_for_one_node(
            connections, node_classification, tested_node_index);

        CHECK_THAT(node_classification,
            Catch::Matchers::RangeEquals(
                vector<node_layer>{node_layer::fine, node_layer::coarse,
                    node_layer::fine, node_layer::coarse, node_layer::coarse}));
    }

    SECTION("change the tested node if two neighboring nodes are not OK") {
        node_connection_list<int> connections;
        connections.push_back(1);
        connections.push_back(2);
        connections.push_back(3);
        connections.push_back(4);
        connections.finish_current_node();
        connections.finish_current_node();
        connections.finish_current_node();
        connections.finish_current_node();
        connections.push_back(0);
        connections.finish_current_node();
        vector<node_layer> node_classification{node_layer::fine,
            node_layer::coarse, node_layer::fine, node_layer::coarse,
            node_layer::fine};
        constexpr num_collect::index_type tested_node_index = 0;

        tune_coarse_grid_selection_for_one_node(
            connections, node_classification, tested_node_index);

        CHECK_THAT(node_classification,
            Catch::Matchers::RangeEquals(
                vector<node_layer>{node_layer::coarse, node_layer::coarse,
                    node_layer::fine, node_layer::coarse, node_layer::fine}));
    }
}

TEST_CASE("num_collect::linear::impl::amg::tune_coarse_grid_selection") {
    using num_collect::linear::impl::amg::node_connection_list;
    using num_collect::linear::impl::amg::node_layer;
    using num_collect::linear::impl::amg::tune_coarse_grid_selection;
    using num_collect::linear::impl::amg::
        tune_coarse_grid_selection_for_one_node;
    using num_collect::util::vector;

    SECTION("apply to laplacian_2d_grid") {
        using num_prob_collect::linear::laplacian_2d_grid;
        using scalar_type = double;
        using matrix_type = Eigen::SparseMatrix<scalar_type>;
        using storage_index_type = typename matrix_type::StorageIndex;
        using num_collect::linear::impl::amg::build_first_coarse_grid_candidate;
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
        auto node_classification = build_first_coarse_grid_candidate(
            connections, transposed_connections);

        node_classification[57] = node_layer::fine;  // NOLINT

        tune_coarse_grid_selection(
            connections, transposed_connections, node_classification);

        fmt::memory_buffer buffer;
        buffer.append(std::string_view("Classification:"));
        for (num_collect::index_type i = 0; i < node_classification.size();
             ++i) {
            if (i % grid_size == 0U) {
                buffer.push_back('\n');
            }
            if (node_classification[i] == node_layer::coarse) {
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
