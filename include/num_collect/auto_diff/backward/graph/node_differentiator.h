/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of node class.
 */
#pragma once

#include <queue>
#include <unordered_map>

#include "num_collect/auto_diff/backward/graph/node.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/index_type.h"

namespace num_collect::auto_diff::backward::graph {

/*!
 * \brief Class to compute differential coefficients for nodes in backward-mode
 * automatic differentiation \cite Kubota1998.
 *
 * \tparam Scalar Type of scalars.
 */
template <typename Scalar>
class node_differentiator {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Construct.
     */
    node_differentiator() = default;

    /*!
     * \brief Compute differential coefficients.
     *
     * \param[in] top_node Top node to differentiate.
     */
    void compute(const node_ptr<scalar_type>& top_node) {
        NUM_COLLECT_ASSERT(top_node);
        list_nodes(top_node);
        compute_coeffs(top_node);
    }

    /*!
     * \brief Get the differential coefficient of a node.
     *
     * \param[in] node Node.
     * \return Differential coefficient of the node.
     */
    [[nodiscard]] auto coeff(const node_ptr<scalar_type>& node) const {
        NUM_COLLECT_DEBUG_ASSERT(node);
        if (const auto iter = info_dict_.find(node); iter != info_dict_.end()) {
            return iter->second.diff;
        }
        return static_cast<scalar_type>(0);
    }

private:
    //! Struct to save internal information of nodes.
    struct node_info {
        //! Differential coefficient.
        scalar_type diff{static_cast<scalar_type>(0)};

        //! Number of remaining references.
        index_type ref_count{0};
    };

    /*!
     * \brief List nodes.
     *
     * \param[in] top_node Top node to differentiate.
     */
    void list_nodes(const node_ptr<scalar_type>& top_node) {
        info_dict_.clear();
        node_queue_ = std::queue<node_ptr<scalar_type>>();

        info_dict_.try_emplace(top_node);
        node_queue_.push(top_node);
        while (!node_queue_.empty()) {
            const auto& node = node_queue_.front();
            for (const auto& child_node : node->children()) {
                const auto [iter, inserted] =
                    info_dict_.try_emplace(child_node.node());
                if (inserted) {
                    node_queue_.push(child_node.node());
                }
                ++iter->second.ref_count;
            }
            node_queue_.pop();
        }
    }

    /*!
     * \brief Compute the differential coefficients using the information
     * generated by list_nodes function.
     *
     * \param[in] top_node Top node to differentiate.
     */
    void compute_coeffs(const node_ptr<scalar_type>& top_node) {
        info_dict_[top_node].diff = static_cast<scalar_type>(1);
        node_queue_.push(top_node);
        while (!node_queue_.empty()) {
            const auto& node = node_queue_.front();
            const auto& info = info_dict_[node];
            for (const auto& child_node : node->children()) {
                auto& child_info = info_dict_[child_node.node()];
                child_info.diff += info.diff * child_node.sensitivity();
                --child_info.ref_count;
                if (child_info.ref_count == 0) {
                    node_queue_.push(child_node.node());
                }
            }
            node_queue_.pop();
        }
    }

    //! Dictionally of information of nodes.
    std::unordered_map<node_ptr<scalar_type>, node_info> info_dict_{};

    //! Queue of remaining nodes.
    std::queue<node_ptr<scalar_type>> node_queue_{};
};

}  // namespace num_collect::auto_diff::backward::graph
