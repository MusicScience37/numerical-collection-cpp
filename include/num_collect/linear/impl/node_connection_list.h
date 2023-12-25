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
 * \brief Definition of node_connection_list class.
 */
#pragma once

#include <span>
#include <vector>

#include "num_collect/base/index_type.h"

namespace num_collect::linear::impl {

/*!
 * \brief Class of lists of connected nodes per node.
 *
 * This class lists the list of connected nodes per node.
 *
 * \tparam StorageIndex Type of indices in storages.
 */
template <typename StorageIndex = int>
class node_connection_list {
public:
    //! Type of indices in storages.
    using storage_index_type = StorageIndex;

    /*!
     * \brief Constructor.
     */
    node_connection_list() {
        begin_indices_.push_back(static_cast<storage_index_type>(0));
    }

    /*!
     * \name Functions to populate this list.
     */
    ///@{

    /*!
     * \brief Initialize this list.
     *
     * \note An object after construction is in the same state as after a call
     * to this function.
     */
    void initialize() {
        node_indices_.clear();
        begin_indices_.clear();
        begin_indices_.push_back(static_cast<storage_index_type>(0));
    }

    /*!
     * \brief Add a connected node to the current node.
     *
     * \param[in] node_index Index of the connected node.
     */
    void push_back(storage_index_type node_index) {
        node_indices_.push_back(node_index);
    }

    /*!
     * \brief Finish adding connected nodes to the current node and start the
     * next node.
     */
    void finish_current_node() {
        begin_indices_.push_back(
            static_cast<storage_index_type>(node_indices_.size()));
    }

    ///@}

    /*!
     * \name Functions to access this list.
     */
    ///@{

    /*!
     * \brief Get the number of nodes.
     *
     * \return Number of nodes.
     */
    [[nodiscard]] auto num_nodes() const -> index_type {
        return static_cast<index_type>(begin_indices_.size()) -
            static_cast<index_type>(1);
    }

    /*!
     * \brief Get the list of indices of connected nodes to the node with the
     * given index.
     *
     * \param[in] node_index Index of the node.
     * \return List of indices of connected nodes.
     */
    [[nodiscard]] auto connected_nodes_to(index_type node_index) const
        -> std::span<const storage_index_type> {
        return std::span<const storage_index_type>(node_indices_.data() +
                begin_indices_[static_cast<std::size_t>(node_index)],
            node_indices_.data() +
                begin_indices_[static_cast<std::size_t>(node_index + 1)]);
    }

    ///@}

private:
    //! Indices of connected nodes.
    std::vector<storage_index_type> node_indices_{};

    //! List of indices of beginning of the list of connected nodes per node.
    std::vector<storage_index_type> begin_indices_{};
};

}  // namespace num_collect::linear::impl
