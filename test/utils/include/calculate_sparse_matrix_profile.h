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
 * \brief Definition of calculate_sparse_matrix_profile function.
 */
#pragma once

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"

/*!
 * \brief Calculate the profile of a sparse matrix.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] matrix Matrix.
 * \return Profile.
 */
template <num_collect::concepts::sparse_matrix Matrix>
[[nodiscard]] inline auto calculate_sparse_matrix_profile(
    const Matrix& matrix) -> num_collect::index_type {
    const num_collect::index_type size = matrix.outerSize();
    num_collect::index_type profile = size;
    for (num_collect::index_type i = 0; i < size; ++i) {
        num_collect::index_type min_ind = i;
        for (typename Matrix::InnerIterator iter(matrix, i); iter; ++iter) {
            if (iter.index() < min_ind) {
                min_ind = iter.index();
            }
        }
        profile += i - min_ind;
    }
    return profile;
}
