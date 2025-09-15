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
 * \brief Definition of sparse_diff_matrix_2d function.
 */
#pragma once

#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/vector.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Create a sparse differential matrix in 2D.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] outer_size Size of the outer dimension.
 * \param[in] inner_size Size of the inner dimension.
 * \return Matrix.
 */
template <num_collect::concepts::sparse_matrix Matrix>
[[nodiscard]] auto sparse_diff_matrix_2d(num_collect::index_type outer_size,
    num_collect::index_type inner_size) -> Matrix {
    using scalar_type = typename Matrix::value_type;
    using storage_index_type = typename Matrix::StorageIndex;

    const num_collect::index_type rows =
        (outer_size - 1) * inner_size + outer_size * (inner_size - 1);
    const num_collect::index_type cols = outer_size * inner_size;

    // Difference for the outer index.
    num_collect::util::vector<Eigen::Triplet<scalar_type, storage_index_type>>
        triplets;
    for (num_collect::index_type o = 0; o < outer_size - 1; ++o) {
        for (num_collect::index_type i = 0; i < inner_size; ++i) {
            const num_collect::index_type row = o * inner_size + i;
            triplets.emplace_back(static_cast<storage_index_type>(row),
                static_cast<storage_index_type>(o * inner_size + i),
                static_cast<scalar_type>(1));
            triplets.emplace_back(static_cast<storage_index_type>(row),
                static_cast<storage_index_type>((o + 1) * inner_size + i),
                static_cast<scalar_type>(-1));
        }
    }

    // Difference for the inner index.
    for (num_collect::index_type o = 0; o < outer_size; ++o) {
        for (num_collect::index_type i = 0; i < inner_size - 1; ++i) {
            const num_collect::index_type row =
                (outer_size - 1) * inner_size + o * (inner_size - 1) + i;
            triplets.emplace_back(static_cast<storage_index_type>(row),
                static_cast<storage_index_type>(o * inner_size + i),
                static_cast<scalar_type>(1));
            triplets.emplace_back(static_cast<storage_index_type>(row),
                static_cast<storage_index_type>(o * inner_size + i + 1),
                static_cast<scalar_type>(-1));
        }
    }

    Matrix matrix(rows, cols);
    matrix.setFromTriplets(triplets.begin(), triplets.end());
    return matrix;
}

}  // namespace num_prob_collect::regularization
