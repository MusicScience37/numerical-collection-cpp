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
 * \brief Definition of format_non_zeros function.
 */
#pragma once

#include <string>

#include <fmt/format.h>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"

/*!
 * \brief Format non-zeros of a sparse matrix..
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] matrix Matrix.
 * \return Formatted string.
 */
template <num_collect::concepts::sparse_matrix Matrix>
[[nodiscard]] inline auto format_non_zeros(const Matrix& matrix)
    -> std::string {
    using scalar_type = typename Matrix::Scalar;

    fmt::memory_buffer buffer;

    const num_collect::index_type rows = matrix.rows();
    const num_collect::index_type cols = matrix.cols();
    for (num_collect::index_type i = 0; i < rows; ++i) {
        for (num_collect::index_type j = 0; j < cols; ++j) {
            const scalar_type value = matrix.coeff(i, j);
            if (value != static_cast<scalar_type>(0)) {
                buffer.push_back('x');
            } else {
                buffer.push_back('-');
            }
        }
        buffer.push_back('\n');
    }

    return std::string(buffer.data(), buffer.size());
}
