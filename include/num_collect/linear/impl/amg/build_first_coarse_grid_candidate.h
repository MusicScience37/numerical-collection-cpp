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

#include <cstddef>
#include <optional>
#include <vector>

#include "num_collect/linear/impl/amg/grid_type.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"

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
    -> std::vector<StorageIndex> {
    std::vector<StorageIndex> scores(
        static_cast<std::size_t>(transposed_connections.num_nodes()));
    for (StorageIndex i = 0; i < transposed_connections.num_nodes(); ++i) {
        scores[i] = static_cast<StorageIndex>(
            transposed_connections.connected_nodes_to(i).size());
    }
    return scores;
}

/*!
 * \brief Find the index of the maximum score among unclassified nodes.
 *
 * \tparam StorageIndex Type of indices in storage.
 * \param[in] scores Current score.
 * \param[in] classification Classification of nodes.
 * \return Index. (Null if no unclassified nodes.)
 */
template <typename StorageIndex>
[[nodiscard]] auto find_max_score_index(const std::vector<StorageIndex>& scores,
    const std::vector<grid_type>& classification)
    -> std::optional<std::size_t> {
    std::optional<std::size_t> index;
    auto score = static_cast<StorageIndex>(-1);
    for (std::size_t i = 0; i < scores.size(); ++i) {
        if (classification[i] == grid_type::unclassified) {
            if (scores[i] > score) {
                index = i;
                score = scores[i];
            }
        }
    }
    return index;
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
    -> std::vector<grid_type> {
    std::vector<grid_type> classification(
        static_cast<std::size_t>(connections.num_nodes()),
        grid_type::unclassified);

    std::vector<StorageIndex> scores =
        compute_node_scores(transposed_connections);

    while (true) {
        auto selection = find_max_score_index(scores, classification);
        if (!selection) {
            break;
        }

        classification[*selection] = grid_type::coarse;
        for (const auto j :
            transposed_connections.connected_nodes_to(*selection)) {
            if (classification[static_cast<std::size_t>(j)] ==
                grid_type::unclassified) {
                classification[static_cast<std::size_t>(j)] = grid_type::fine;
                for (const auto k : connections.connected_nodes_to(j)) {
                    scores[static_cast<std::size_t>(k)] += 1;
                }
            }
        }
        for (const auto j : connections.connected_nodes_to(*selection)) {
            if (classification[static_cast<std::size_t>(j)] ==
                grid_type::unclassified) {
                scores[static_cast<std::size_t>(j)] -= 1;
            }
        }
    }

    return classification;
}

}  // namespace num_collect::linear::impl::amg
