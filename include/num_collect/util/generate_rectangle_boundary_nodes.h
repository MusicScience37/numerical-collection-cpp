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
 * \brief Definition of generate_rectangle_boundary_nodes function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/util/vector.h"

namespace num_collect::util {

/*!
 * \brief Generate nodes on the boundary of a rectangle.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] lower_left_node Node at the lower left corner of the rectangle.
 * \param[in] upper_right_node Node at the upper right corner of the rectangle.
 * \param[in] num_nodes_per_edge Number of nodes on each edge of the rectangle.
 * \param[in] order_in_clockwise Whether the order of nodes is clockwise.
 * \return Generated nodes.
 */
template <base::concepts::real_scalar Scalar>
[[nodiscard]] auto generate_rectangle_boundary_nodes(
    const Eigen::Vector2<Scalar>& lower_left_node,
    const Eigen::Vector2<Scalar>& upper_right_node,
    index_type num_nodes_per_edge, bool order_in_clockwise = false)
    -> vector<Eigen::Vector2<Scalar>> {
    NUM_COLLECT_PRECONDITION(
        num_nodes_per_edge >= 1, "num_nodes_per_edge must be at least 1.");

    const Scalar x_min = lower_left_node.x();
    const Scalar y_min = lower_left_node.y();
    const Scalar x_max = upper_right_node.x();
    const Scalar y_max = upper_right_node.y();
    const Scalar dx = (x_max - x_min) / static_cast<Scalar>(num_nodes_per_edge);
    const Scalar dy = (y_max - y_min) / static_cast<Scalar>(num_nodes_per_edge);

    vector<Eigen::Vector2<Scalar>> nodes;
    nodes.reserve(4 * num_nodes_per_edge);

    if (order_in_clockwise) {
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_min, y_min + static_cast<Scalar>(i) * dy));
        }
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_min + static_cast<Scalar>(i) * dx, y_max));
        }
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_max, y_max - static_cast<Scalar>(i) * dy));
        }
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_max - static_cast<Scalar>(i) * dx, y_min));
        }
    } else {
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_min + static_cast<Scalar>(i) * dx, y_min));
        }
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_max, y_min + static_cast<Scalar>(i) * dy));
        }
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_max - static_cast<Scalar>(i) * dx, y_max));
        }
        for (index_type i = 0; i < num_nodes_per_edge; ++i) {
            nodes.push_back(Eigen::Vector2<Scalar>(
                x_min, y_max - static_cast<Scalar>(i) * dy));
        }
    }

    return nodes;
}

}  // namespace num_collect::util
