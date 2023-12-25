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
 * \brief Definition of compute_strong_connection_list function.
 */
#pragma once

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/impl/node_connection_list.h"

namespace num_collect::linear::impl {

/*!
 * \brief Compute a list of strong connections in a matrix \cite Ruge1987.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] matrix Matrix.
 * \param[in] strong_coeff_rate_threshold Threshold of the rate of coefficients
 * to determine strong connections.
 * \return List of strong connections.
 */
template <base::concepts::sparse_matrix Matrix>
inline auto compute_strong_connection_list(
    const Matrix& matrix, typename Matrix::Scalar strong_coeff_rate_threshold)
    -> node_connection_list<typename Matrix::StorageIndex> {
    using scalar_type = typename Matrix::Scalar;
    using std::abs;

    node_connection_list<typename Matrix::StorageIndex> list;

    const index_type num_nodes = matrix.outerSize();
    for (index_type i = 0; i < num_nodes; ++i) {
        auto max_coeff = static_cast<scalar_type>(0);
        for (typename Matrix::InnerIterator iter(matrix, i); iter; ++iter) {
            const auto abs_coeff = abs(iter.value());
            if (iter.index() != i && abs_coeff > max_coeff) {
                max_coeff = abs_coeff;
            }
        }
        const auto strong_coeff_threshold =
            strong_coeff_rate_threshold * max_coeff;
        for (typename Matrix::InnerIterator iter(matrix, i); iter; ++iter) {
            if (iter.index() != i &&
                abs(iter.value()) >= strong_coeff_threshold) {
                list.push_back(iter.index());
            }
        }
        list.finish_current_node();
    }

    return list;
}

}  // namespace num_collect::linear::impl
