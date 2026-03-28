/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of generate_cuboid_boundary_nodes function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"

namespace num_collect::util {

/*!
 * \brief Generate nodes on the boundary of a cuboid.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] lower_boundary_node Node with lower boundary coordinates of the
 * cuboid.
 * \param[in] upper_boundary_node Node with upper boundary coordinates of the
 * cuboid.
 * \param[in] num_nodes_per_edge Number of nodes on each edge of rectangle
 * surfaces of the cuboid.
 * \return Generated nodes.
 */
template <base::concepts::real_scalar Scalar>
[[nodiscard]] auto generate_cuboid_boundary_nodes(
    const Eigen::Vector3<Scalar>& lower_boundary_node,
    const Eigen::Vector3<Scalar>& upper_boundary_node,
    index_type num_nodes_per_edge) -> vector<Eigen::Vector3<Scalar>> {
    NUM_COLLECT_PRECONDITION(
        num_nodes_per_edge >= 1, "num_nodes_per_edge must be at least 1.");
    NUM_COLLECT_PRECONDITION(num_nodes_per_edge <= 32768,
        "num_nodes_per_edge over 32768 is not supported to avoid overflows.");

    const Scalar x_min = lower_boundary_node.x();
    const Scalar y_min = lower_boundary_node.y();
    const Scalar z_min = lower_boundary_node.z();
    const Scalar x_max = upper_boundary_node.x();
    const Scalar y_max = upper_boundary_node.y();
    const Scalar z_max = upper_boundary_node.z();
    const Scalar dx = (x_max - x_min) / static_cast<Scalar>(num_nodes_per_edge);
    const Scalar dy = (y_max - y_min) / static_cast<Scalar>(num_nodes_per_edge);
    const Scalar dz = (z_max - z_min) / static_cast<Scalar>(num_nodes_per_edge);

    vector<Eigen::Vector3<Scalar>> nodes;
    const index_type num_total_nodes =
        // Bottom and top faces
        (num_nodes_per_edge + 1) * (num_nodes_per_edge + 1) * 2
        // Other four faces
        + 4 * num_nodes_per_edge * (num_nodes_per_edge - 1);
    nodes.reserve(num_total_nodes);

    // Bottom and top faces
    for (index_type i = 0; i <= num_nodes_per_edge; ++i) {
        for (index_type j = 0; j <= num_nodes_per_edge; ++j) {
            nodes.push_back(
                Eigen::Vector3<Scalar>(x_min + static_cast<Scalar>(i) * dx,
                    y_min + static_cast<Scalar>(j) * dy, z_min));
            nodes.push_back(
                Eigen::Vector3<Scalar>(x_min + static_cast<Scalar>(i) * dx,
                    y_min + static_cast<Scalar>(j) * dy, z_max));
        }
    }
    // Other four faces
    for (index_type k = 1; k < num_nodes_per_edge; ++k) {
        const Scalar z = z_min + static_cast<Scalar>(k) * dz;
        for (index_type i = 0; i <= num_nodes_per_edge; ++i) {
            const Scalar x = x_min + static_cast<Scalar>(i) * dx;
            nodes.push_back(Eigen::Vector3<Scalar>(x, y_min, z));
            nodes.push_back(Eigen::Vector3<Scalar>(x, y_max, z));
        }
        for (index_type i = 1; i < num_nodes_per_edge; ++i) {
            const Scalar y = y_min + static_cast<Scalar>(i) * dy;
            nodes.push_back(Eigen::Vector3<Scalar>(x_min, y, z));
            nodes.push_back(Eigen::Vector3<Scalar>(x_max, y, z));
        }
    }

    NUM_COLLECT_DEBUG_ASSERT(nodes.size() == num_total_nodes);

    return nodes;
}

}  // namespace num_collect::util
