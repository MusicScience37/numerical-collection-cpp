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
 * \brief Definition of build_first_coarse_grid_candidate function.
 */
#pragma once

#include <limits>
#include <optional>

#include "num_collect/base/index_type.h"
#include "num_collect/linear/impl/amg/index_score_table.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"
#include "num_collect/linear/impl/amg/node_layer.h"
#include "num_collect/util/vector.h"

namespace num_collect::linear::impl::amg {

/*!
 * \brief Compute the first scores of nodes.
 *
 * \tparam StorageIndex Type of indices in storage.
 * \param[in] transposed_connections Transposed list of connections.
 * \return Scores.
 */
template <typename StorageIndex>
[[nodiscard]] inline auto compute_node_scores(
    const node_connection_list<StorageIndex>& transposed_connections)
    -> index_score_table<StorageIndex> {
    index_score_table<StorageIndex> scores(transposed_connections.num_nodes());
    for (StorageIndex i = 0; i < transposed_connections.num_nodes(); ++i) {
        scores.assign(i,
            static_cast<StorageIndex>(
                transposed_connections.connected_nodes_to(i).size()));
    }
    return scores;
}

/*!
 * \brief Build the first candidate of a coarse grid.
 *
 * \tparam StorageIndex Type of indices in storage.
 * \param[in] connections List of connections.
 * \param[in] transposed_connections Transposed list of connections.
 * \return Classification of nodes.
 */
template <typename StorageIndex>
[[nodiscard]] inline auto build_first_coarse_grid_candidate(
    const node_connection_list<StorageIndex>& connections,
    const node_connection_list<StorageIndex>& transposed_connections)
    -> util::vector<node_layer> {
    util::vector<node_layer> classification(
        connections.num_nodes(), node_layer::unclassified);

    index_score_table<StorageIndex> table =
        compute_node_scores(transposed_connections);

    while (!table.empty()) {
        auto selection = table.find_max_score_index();

        classification[selection] = node_layer::coarse;
        table.remove(selection);
        for (const auto j :
            transposed_connections.connected_nodes_to(selection)) {
            if (classification[j] == node_layer::unclassified) {
                classification[j] = node_layer::fine;
                table.remove(j);
                for (const auto k : connections.connected_nodes_to(j)) {
                    table.add_score(k, 1);
                }
            }
        }
        for (const auto j : connections.connected_nodes_to(selection)) {
            if (classification[j] == node_layer::unclassified) {
                table.add_score(j, -1);
            }
        }
    }

    return classification;
}

}  // namespace num_collect::linear::impl::amg
