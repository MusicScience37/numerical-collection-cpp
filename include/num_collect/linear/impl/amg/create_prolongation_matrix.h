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
 * \brief Definition of create_prolongation_matrix function.
 */
#pragma once

#include <vector>

#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"
#include "num_collect/linear/impl/amg/node_layer.h"
#include "num_collect/util/vector.h"

namespace num_collect::linear::impl::amg {

/*!
 * \brief Create a prolongation matrix.
 *
 * \tparam Matrix Type of the prolongation matrix.
 * \param[out] prolongation_matrix Prolongation matrix.
 * \param[in] transposed_connections Transposed list of connections.
 * \param[in] node_classification Classification of nodes.
 */
template <base::concepts::sparse_matrix Matrix>
void create_prolongation_matrix(Matrix& prolongation_matrix,
    const node_connection_list<typename Matrix::StorageIndex>&
        transposed_connections,
    const util::vector<node_layer>& node_classification) {
    using storage_index_type = typename Matrix::StorageIndex;
    using scalar_type = typename Matrix::Scalar;

    const auto num_nodes =
        static_cast<storage_index_type>(node_classification.size());

    util::vector<storage_index_type> coarse_grid_indices;
    storage_index_type num_coarse_grid_nodes = 0;
    coarse_grid_indices.reserve(num_nodes);
    for (const node_layer node_class : node_classification) {
        if (node_class == node_layer::coarse) {
            coarse_grid_indices.push_back(num_coarse_grid_nodes);
            ++num_coarse_grid_nodes;
        } else {
            coarse_grid_indices.push_back(num_nodes);
        }
    }

    std::vector<Eigen::Triplet<scalar_type, storage_index_type>> triplets;
    for (storage_index_type row_index = 0; row_index < num_nodes; ++row_index) {
        if (node_classification[row_index] == node_layer::coarse) {
            const storage_index_type col_index = coarse_grid_indices[row_index];
            const auto value = static_cast<scalar_type>(1);
            triplets.emplace_back(row_index, col_index, value);
        } else {
            storage_index_type num_connected_nodes = 0;
            for (const storage_index_type connected_node_index :
                transposed_connections.connected_nodes_to(row_index)) {
                if (node_classification[connected_node_index] ==
                    node_layer::coarse) {
                    ++num_connected_nodes;
                }
            }

            const scalar_type value = static_cast<scalar_type>(1) /
                static_cast<scalar_type>(num_connected_nodes);
            for (const storage_index_type connected_node_index :
                transposed_connections.connected_nodes_to(row_index)) {
                if (node_classification[connected_node_index] ==
                    node_layer::coarse) {
                    const storage_index_type col_index =
                        coarse_grid_indices[connected_node_index];
                    triplets.emplace_back(row_index, col_index, value);
                }
            }
        }
    }

    prolongation_matrix.resize(num_nodes, num_coarse_grid_nodes);
    prolongation_matrix.setFromTriplets(triplets.begin(), triplets.end());
}

}  // namespace num_collect::linear::impl::amg
