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
 * \brief Test of create_prolongation_matrix function.
 */
#include "num_collect/linear/impl/amg/create_prolongation_matrix.h"

#include <ApprovalTests.hpp>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/linear/impl/amg/node_connection_list.h"
#include "num_collect/linear/impl/amg/node_layer.h"
#include "num_collect/util/format_sparse_matrix.h"

TEST_CASE("num_collect::linear::impl::amg::create_prolongation_matrix") {
    using num_collect::linear::impl::amg::create_prolongation_matrix;
    using num_collect::linear::impl::amg::node_connection_list;
    using num_collect::linear::impl::amg::node_layer;

    SECTION("create a prolongation matrix") {
        const auto node_classification =
            num_collect::util::vector{node_layer::fine, node_layer::coarse,
                node_layer::coarse, node_layer::coarse, node_layer::fine};
        node_connection_list<int> transposed_connections;
        transposed_connections.push_back(1);
        transposed_connections.push_back(3);
        transposed_connections.push_back(4);
        transposed_connections.finish_current_node();
        transposed_connections.finish_current_node();
        transposed_connections.finish_current_node();
        transposed_connections.finish_current_node();
        transposed_connections.push_back(1);
        transposed_connections.push_back(2);
        transposed_connections.push_back(3);
        transposed_connections.finish_current_node();

        Eigen::SparseMatrix<double> prolongation_matrix;
        create_prolongation_matrix(
            prolongation_matrix, transposed_connections, node_classification);

        ApprovalTests::Approvals::verify(fmt::format("{:.3f}",
            num_collect::util::format_sparse_matrix(prolongation_matrix,
                num_collect::util::sparse_matrix_format_type::multi_line)));
    }
}
