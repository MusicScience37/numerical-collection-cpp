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
 * \brief Definition of sparse_downsampling_matrix_2d function.
 */
#pragma once

#include <algorithm>

#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/util/vector.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Create a sparse matrix for downsampling of 2D images.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] original_outer_size Size of the outer dimension of the original
 * image.
 * \param[in] original_inner_size Size of the inner dimension of the original
 * image.
 * \param[in] downsampled_outer_size Size of the outer dimension of the
 * downsampled image.
 * \param[in] downsampled_inner_size Size of the inner dimension of the
 * downsampled image.
 * \return Created sparse matrix.
 */
template <num_collect::concepts::sparse_matrix Matrix>
[[nodiscard]] auto sparse_downsampling_matrix_2d(
    num_collect::index_type original_outer_size,
    num_collect::index_type original_inner_size,
    num_collect::index_type downsampled_outer_size,
    num_collect::index_type downsampled_inner_size) -> Matrix {
    using scalar_type = typename Matrix::value_type;
    using storage_index_type = typename Matrix::StorageIndex;

    // NOLINTNEXTLINE(readability-simplify-boolean-expr): Impossible to fix this case.
    NUM_COLLECT_PRECONDITION(original_outer_size > 0 &&
            original_inner_size > 0 && downsampled_outer_size > 0 &&
            downsampled_inner_size > 0,
        "All sizes must be positive integers.");

    const num_collect::index_type rows =
        downsampled_outer_size * downsampled_inner_size;
    const num_collect::index_type cols =
        original_outer_size * original_inner_size;

    num_collect::util::vector<Eigen::Triplet<scalar_type, storage_index_type>>
        triplets;
    for (num_collect::index_type downsampled_outer_index = 0;
        downsampled_outer_index < downsampled_outer_size;
        ++downsampled_outer_index) {
        const num_collect::index_type original_outer_index_begin =
            downsampled_outer_index * original_outer_size /
            downsampled_outer_size;
        const num_collect::index_type original_outer_index_end =
            std::min((downsampled_outer_index + 1) * original_outer_size /
                    downsampled_outer_size,
                original_outer_size);
        for (num_collect::index_type downsampled_inner_index = 0;
            downsampled_inner_index < downsampled_inner_size;
            ++downsampled_inner_index) {
            const num_collect::index_type original_inner_index_begin =
                downsampled_inner_index * original_inner_size /
                downsampled_inner_size;
            const num_collect::index_type original_inner_index_end =
                std::min((downsampled_inner_index + 1) * original_inner_size /
                        downsampled_inner_size,
                    original_inner_size);
            const num_collect::index_type row =
                downsampled_outer_index * downsampled_inner_size +
                downsampled_inner_index;
            for (num_collect::index_type original_outer_index =
                     original_outer_index_begin;
                original_outer_index < original_outer_index_end;
                ++original_outer_index) {
                for (num_collect::index_type original_inner_index =
                         original_inner_index_begin;
                    original_inner_index < original_inner_index_end;
                    ++original_inner_index) {
                    const num_collect::index_type col =
                        original_outer_index * original_inner_size +
                        original_inner_index;
                    triplets.emplace_back(static_cast<storage_index_type>(row),
                        static_cast<storage_index_type>(col),
                        static_cast<scalar_type>(1) /
                            static_cast<scalar_type>(
                                (original_outer_index_end -
                                    original_outer_index_begin) *
                                (original_inner_index_end -
                                    original_inner_index_begin)));
                }
            }
        }
    }

    Matrix matrix(rows, cols);
    matrix.setFromTriplets(triplets.begin(), triplets.end());
    return matrix;
}

}  // namespace num_prob_collect::regularization
