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
 * \brief Definition of tune_coarse_grid_selection function.
 */
#pragma once

#include <algorithm>
#include <optional>

#include <hash_tables/sets/open_address_set_st.h>

#include "num_collect/base/index_type.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"
#include "num_collect/linear/impl/amg/node_layer.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"

namespace num_collect::linear::impl::amg {

/*!
 * \brief Find a neighboring node unsatisfying the condition of interpolation
 * in \cite Ruge1987 for a node.
 *
 * \tparam StorageIndex Type of indices in storage.
 * \param[in] connections List of connections.
 * \param[in] neighbors_in_coarse_grid Neighboring nodes in coarse grid.
 * \param[in] neighbors_in_fine_grid Neighboring nodes only in finer grid.
 * \return Index of unsatisfying node. (Null if not found.)
 */
template <typename StorageIndex>
[[nodiscard]] inline auto find_node_unsatisfying_interpolation_condition(
    const node_connection_list<StorageIndex>& connections,
    const hash_tables::sets::open_address_set_st<StorageIndex>&
        neighbors_in_coarse_grid,
    const hash_tables::sets::open_address_set_st<StorageIndex>&
        neighbors_in_fine_grid) -> std::optional<StorageIndex> {
    std::optional<StorageIndex> result;
    neighbors_in_fine_grid.for_all(
        [&result, &connections, &neighbors_in_coarse_grid](
            const StorageIndex& neighbor) {
            if (result) {
                return;
            }
            if (std::ranges::none_of(connections.connected_nodes_to(neighbor),
                    [neighbors_in_coarse_grid](StorageIndex index) {
                        return neighbors_in_coarse_grid.has(index);
                    })) {
                result = neighbor;
            }
        });
    return result;
}

/*!
 * \brief Tune a node in a coarse grid to satisfy the condition for
 * interpolation specified in \cite Ruge1987.
 *
 * \tparam StorageIndex Type of indices in storage.
 * \param[in] connections List of connections.
 * \param[in,out] node_classification Classification of nodes.
 * \param[in] tested_node_index Index of the tested node.
 * \param[out] neighbors_in_coarse_grid Buffer of Indices of neighbors in coarse
 * grid.
 * \param[out] neighbors_in_fine_grid Buffer of Indices of neighbors in fine
 * grid.
 */
template <typename StorageIndex>
void tune_coarse_grid_selection_for_one_node(
    const node_connection_list<StorageIndex>& connections,
    util::vector<node_layer>& node_classification, index_type tested_node_index,
    hash_tables::sets::open_address_set_st<StorageIndex>&
        neighbors_in_coarse_grid,
    hash_tables::sets::open_address_set_st<StorageIndex>&
        neighbors_in_fine_grid) {
    NUM_COLLECT_DEBUG_ASSERT(
        node_classification[tested_node_index] == node_layer::fine);

    neighbors_in_coarse_grid.clear();
    neighbors_in_fine_grid.clear();
    for (const auto neighbor :
        connections.connected_nodes_to(tested_node_index)) {
        if (node_classification[neighbor] == node_layer::coarse) {
            neighbors_in_coarse_grid.insert(neighbor);
        } else {
            NUM_COLLECT_DEBUG_ASSERT(
                node_classification[neighbor] == node_layer::fine);
            neighbors_in_fine_grid.insert(neighbor);
        }
    }

    auto unsatisfying_node = find_node_unsatisfying_interpolation_condition(
        connections, neighbors_in_coarse_grid, neighbors_in_fine_grid);
    if (!unsatisfying_node) {
        return;
    }
    // Temporarily change the node.
    neighbors_in_fine_grid.erase(*unsatisfying_node);
    neighbors_in_coarse_grid.insert(*unsatisfying_node);

    if (find_node_unsatisfying_interpolation_condition(
            connections, neighbors_in_coarse_grid, neighbors_in_fine_grid)) {
        // Two nodes are unsatisfying, so move this node to the coarse grid.
        node_classification[tested_node_index] = node_layer::coarse;
    } else {
        // Only one node is unsatisfying, so move the unsatisfying node to the
        // coarse grid.
        node_classification[*unsatisfying_node] = node_layer::coarse;
    }
}

/*!
 * \brief Tune a coarse grid to satisfy the condition for interpolation
 * specified in \cite Ruge1987.
 *
 * \tparam StorageIndex Type of indices in storage.
 * \param[in] connections List of connections.
 * \param[in] transposed_connections Transposed list of connections.
 * \param[in,out] node_classification Classification of nodes.
 */
template <typename StorageIndex>
void tune_coarse_grid_selection(
    const node_connection_list<StorageIndex>& connections,
    const node_connection_list<StorageIndex>& transposed_connections,
    util::vector<node_layer>& node_classification) {
    // At least one node must be strongly connected to a node not in coarse
    // grid.
    for (index_type i = 0; i < node_classification.size(); ++i) {
        if (node_classification[i] != node_layer::coarse) {
            index_type num_connected_nodes = 0;
            for (const index_type connected_node_index :
                transposed_connections.connected_nodes_to(i)) {
                if (node_classification[connected_node_index] ==
                    node_layer::coarse) {
                    ++num_connected_nodes;
                }
            }
            if (num_connected_nodes == 0) {
                node_classification[i] = node_layer::coarse;
            }
        }
    }

    hash_tables::sets::open_address_set_st<StorageIndex>
        neighbors_in_coarse_grid;
    hash_tables::sets::open_address_set_st<StorageIndex> neighbors_in_fine_grid;
    for (index_type i = 0; i < node_classification.size(); ++i) {
        if (node_classification[i] == node_layer::fine) {
            tune_coarse_grid_selection_for_one_node(connections,
                node_classification, i, neighbors_in_coarse_grid,
                neighbors_in_fine_grid);
        }
    }
}

}  // namespace num_collect::linear::impl::amg
