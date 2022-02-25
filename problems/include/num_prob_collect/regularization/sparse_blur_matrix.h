/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of blur_sine class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Create a sparse blur matrix.
 *
 * \tparam Matrix Type of the matrix.
 * \param[out] mat Matrix.
 * \param[in] rows Number of rows. (Size of data.)
 * \param[in] cols Number of columns. (Size of solution.)
 */
template <num_collect::base::concepts::sparse_matrix Matrix>
inline void sparse_blur_matrix(
    Matrix& mat, num_collect::index_type rows, num_collect::index_type cols) {
    using scalar_type = typename Matrix::value_type;
    using storage_index_type = typename Matrix::StorageIndex;

    std::vector<Eigen::Triplet<scalar_type, storage_index_type>> triplets;
    for (num_collect::index_type j = 0; j < cols; ++j) {
        for (num_collect::index_type i = 0; i < rows; ++i) {
            static constexpr auto factor = static_cast<scalar_type>(100.0);
            static constexpr auto thresh = static_cast<scalar_type>(0.1);
            const scalar_type sol_rate =
                static_cast<scalar_type>(j) / static_cast<scalar_type>(cols);
            const scalar_type data_rate =
                static_cast<scalar_type>(i) / static_cast<scalar_type>(rows);
            const scalar_type diff = sol_rate - data_rate;
            const scalar_type coeff =
                std::max(std::exp(-factor * diff * diff) - thresh,
                    static_cast<scalar_type>(0));
            if (coeff > static_cast<scalar_type>(0)) {
                triplets.emplace_back(i, j, coeff);
            }
        }
    }

    mat.resize(rows, cols);
    mat.setFromTriplets(triplets.begin(), triplets.end());
}

}  // namespace num_prob_collect::regularization
