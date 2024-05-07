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
 * \brief Definition of generate_halton_nodes function.
 */
#pragma once

#include <algorithm>
#include <array>
#include <vector>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"

namespace num_collect::rbf {

namespace impl {

/*!
 * \brief Generate a vector of Halton sequence for a single dimension
 * \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 * \param[out] buffer Buffer to generate the sequence.
 * (Values from the second element must be used for the final output.)
 * \param[in] num_nodes Number of nodes.
 * \param[in] base_sequence_size Size of the first sequence.
 */
template <base::concepts::real_scalar Scalar>
void generate_halton_nodes_impl(Eigen::VectorX<Scalar>& buffer,
    index_type num_nodes, index_type base_sequence_size) {
    buffer = Eigen::VectorX<Scalar>::Zero(num_nodes + 1);
    index_type current_size = 1;
    Scalar sequence_interval =
        static_cast<Scalar>(1) / static_cast<Scalar>(base_sequence_size);
    while (current_size <= num_nodes) {
        const index_type current_sequence_size = std::max<index_type>(2,
            std::min<index_type>(
                base_sequence_size, (num_nodes + current_size) / current_size));
        index_type dest_end = current_size;  // save the last index.
        for (index_type i = 1; i < current_sequence_size; ++i) {
            const index_type dest_begin = current_size * i;
            dest_end = dest_begin + current_size;
            if (dest_end > num_nodes + 1 || dest_end < 0) {
                dest_end = num_nodes + 1;
            }
            if (dest_end <= dest_begin) {
                break;
            }
            const index_type dest_size = dest_end - dest_begin;
            const Scalar value_offset =
                static_cast<Scalar>(i) * sequence_interval;
            buffer.segment(dest_begin, dest_size) =
                buffer.head(dest_size).array() + value_offset;
        }
        current_size = dest_end;
        sequence_interval /= static_cast<Scalar>(base_sequence_size);
    }
}

}  // namespace impl

/*!
 * \brief Generate Halton nodes \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Dimensions Number of dimensions of the space
 * in which nodes are generated.
 * \param[in] num_nodes Number of nodes.
 * \return Generated nodes.
 */
template <base::concepts::real_scalar Scalar, index_type Dimensions>
[[nodiscard]] auto generate_halton_nodes(index_type num_nodes)
    -> std::vector<Eigen::Vector<Scalar, Dimensions>> {
    std::vector<Eigen::Vector<Scalar, Dimensions>> nodes(
        static_cast<std::size_t>(num_nodes));

    constexpr index_type supported_dimensions = 6;
    static_assert(Dimensions <= supported_dimensions);
    static_assert(Dimensions > 1);
    constexpr std::array<index_type,
        static_cast<std::size_t>(supported_dimensions)>
        base_sequence_sizes{2, 3, 5, 7, 11, 13};

    Eigen::VectorX<Scalar> buffer;
    for (index_type d = 0; d < Dimensions; ++d) {
        impl::generate_halton_nodes_impl(buffer, num_nodes,
            base_sequence_sizes[static_cast<std::size_t>(d)]);
        for (index_type i = 0; i < num_nodes; ++i) {
            nodes[static_cast<std::size_t>(i)](d) = buffer(i + 1);
        }
    }

    return nodes;
}

/*!
 * \brief Generate Halton nodes in 1 dimension \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] num_nodes Number of nodes.
 * \return Generated nodes.
 */
template <base::concepts::real_scalar Scalar>
[[nodiscard]] auto generate_1d_halton_nodes(index_type num_nodes)
    -> std::vector<Scalar> {
    Eigen::VectorX<Scalar> buffer;
    constexpr index_type base_sequence_size = 2;
    impl::generate_halton_nodes_impl(buffer, num_nodes, base_sequence_size);

    std::vector<Scalar> nodes;
    nodes.reserve(static_cast<std::size_t>(num_nodes));
    for (index_type i = 1; i <= num_nodes; ++i) {
        nodes.push_back(buffer(i));
    }
    return nodes;
}

}  // namespace num_collect::rbf
