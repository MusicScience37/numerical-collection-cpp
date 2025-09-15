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
 * \brief Definition of sparse_diff_matrix function.
 */
#pragma once

#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/vector.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Create a sparse differential matrix.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] size Size (number of columns).
 * \return Matrix.
 */
template <num_collect::concepts::sparse_matrix Matrix>
[[nodiscard]] auto sparse_diff_matrix(num_collect::index_type size) -> Matrix {
    using scalar_type = typename Matrix::value_type;
    using storage_index_type = typename Matrix::StorageIndex;

    const num_collect::index_type rows = size - 1;
    const num_collect::index_type cols = size;

    num_collect::util::vector<Eigen::Triplet<scalar_type, storage_index_type>>
        triplets;
    for (num_collect::index_type i = 0; i < rows; ++i) {
        triplets.emplace_back(static_cast<storage_index_type>(i),
            static_cast<storage_index_type>(i), static_cast<scalar_type>(1));
        triplets.emplace_back(static_cast<storage_index_type>(i),
            static_cast<storage_index_type>(i + 1),
            static_cast<scalar_type>(-1));
    }

    Matrix matrix(rows, cols);
    matrix.setFromTriplets(triplets.begin(), triplets.end());
    return matrix;
}

}  // namespace num_prob_collect::regularization
