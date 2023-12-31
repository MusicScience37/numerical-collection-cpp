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
 * \brief Definition of create_coarse_grid function.
 */
#pragma once

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/linear/impl/amg/build_first_coarse_grid_candidate.h"
#include "num_collect/linear/impl/amg/compute_strong_connection_list.h"
#include "num_collect/linear/impl/amg/node_connection_list.h"
#include "num_collect/linear/impl/amg/node_layer.h"
#include "num_collect/linear/impl/amg/tune_coarse_grid_selection.h"
#include "num_collect/util/vector.h"

namespace num_collect::linear::impl::amg {

/*!
 * \brief Create a coarse grid from a sparse matrix using an algorithm in \cite
 * Ruge1987.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] matrix Matrix.
 * \param[in] strong_coeff_rate_threshold Threshold of the rate of coefficients
 * to determine strong connections.
 * \return Node classification.
 */
template <base::concepts::sparse_matrix Matrix>
[[nodiscard]] inline auto create_coarse_grid(
    const Matrix& matrix, typename Matrix::Scalar strong_coeff_rate_threshold)
    -> util::vector<node_layer> {
    const auto connections =
        compute_strong_connection_list(matrix, strong_coeff_rate_threshold);
    const auto transposed_connections = connections.transpose();
    auto node_classification =
        build_first_coarse_grid_candidate(connections, transposed_connections);
    tune_coarse_grid_selection(connections, node_classification);
    return node_classification;
}

}  // namespace num_collect::linear::impl::amg
