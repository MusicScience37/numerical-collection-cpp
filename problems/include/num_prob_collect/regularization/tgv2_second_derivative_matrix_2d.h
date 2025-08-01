/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of the tgv2_second_derivative_matrix_2d function.
 */
#pragma once

#include <vector>

#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Create a second derivative matrix for 2nd order TGV regularization in
 * 2D images.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] outer_size Size of the outer dimension.
 * \param[in] inner_size Size of the inner dimension.
 * \return Matrix.
 *
 * When \f$D_o\f$ is the difference matrix for the outer index
 * and \f$D_i\f$ is the difference matrix for the inner index,
 * \ref sparse_diff_matrix_2d function creates the matrix
 *
 * \f[
 * \begin{pmatrix}
 *     D_o \\
 *     D_i
 * \end{pmatrix}
 * \f]
 *
 * and this function creates the matrix
 *
 * \f[
 * \begin{pmatrix}
 *     D_o & O \\
 *     O & D_i \\
 *     D_i & D_o
 * \end{pmatrix}
 * \f]
 *
 * where \f$O\f$ is the zero matrix.
 */
template <num_collect::concepts::sparse_matrix Matrix>
[[nodiscard]] auto tgv2_second_derivative_matrix_2d(
    num_collect::index_type outer_size, num_collect::index_type inner_size)
    -> Matrix {
    using scalar_type = typename Matrix::value_type;
    using storage_index_type = typename Matrix::StorageIndex;

    const num_collect::index_type rows = (outer_size - 2) * inner_size +
        outer_size * (inner_size - 2) + (outer_size - 1) * (inner_size - 1);
    const num_collect::index_type cols =
        (outer_size - 1) * inner_size + outer_size * (inner_size - 1);

    // Difference for the outer index to the difference of the outer index.
    std::vector<Eigen::Triplet<scalar_type, storage_index_type>> triplets;
    for (num_collect::index_type o = 0; o < outer_size - 2; ++o) {
        for (num_collect::index_type i = 0; i < inner_size; ++i) {
            const num_collect::index_type row = o * inner_size + i;
            triplets.emplace_back(
                row, o * inner_size + i, static_cast<scalar_type>(1));
            triplets.emplace_back(
                row, (o + 1) * inner_size + i, static_cast<scalar_type>(-1));
        }
    }

    // Difference for the inner index to the difference of the inner index.
    for (num_collect::index_type o = 0; o < outer_size; ++o) {
        for (num_collect::index_type i = 0; i < inner_size - 2; ++i) {
            const num_collect::index_type row =
                (outer_size - 2) * inner_size + o * (inner_size - 2) + i;
            triplets.emplace_back(row,
                (outer_size - 1) * inner_size + o * (inner_size - 1) + i,
                static_cast<scalar_type>(1));
            triplets.emplace_back(row,
                (outer_size - 1) * inner_size + o * (inner_size - 1) + i + 1,
                static_cast<scalar_type>(-1));
        }
    }

    // Mixed differences.
    for (num_collect::index_type o = 0; o < outer_size - 1; ++o) {
        for (num_collect::index_type i = 0; i < inner_size - 1; ++i) {
            const num_collect::index_type row = (outer_size - 2) * inner_size +
                outer_size * (inner_size - 2) + o * (inner_size - 1) + i;
            triplets.emplace_back(
                row, o * inner_size + i, static_cast<scalar_type>(1));
            triplets.emplace_back(
                row, o * inner_size + i + 1, static_cast<scalar_type>(-1));
            triplets.emplace_back(row,
                (outer_size - 1) * inner_size + o * (inner_size - 1) + i,
                static_cast<scalar_type>(1));
            triplets.emplace_back(row,
                (outer_size - 1) * inner_size + (o + 1) * (inner_size - 1) + i,
                static_cast<scalar_type>(-1));
        }
    }

    Matrix matrix(rows, cols);
    matrix.setFromTriplets(triplets.begin(), triplets.end());
    return matrix;
}

}  // namespace num_prob_collect::regularization
