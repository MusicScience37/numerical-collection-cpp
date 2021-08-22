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
 * \brief Definition of dense_diff_matrix function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/util/assert.h"
#include "num_collect/util/index_type.h"

namespace num_prob_collect::regularization {

namespace impl {

/*!
 * \brief Helper class to create a dense differential matrix.
 *
 * \tparam Matrix Type of matrices.
 */
template <typename Matrix>
class dense_diff_matrix_functor {
public:
    //! Type of scalars.
    using scalar_type = typename Matrix::Scalar;

    /*!
     * \brief Construct.
     */
    dense_diff_matrix_functor() = default;

    /*!
     * \brief Get an element.
     *
     * \param[in] row Row index.
     * \param[in] col Column index.
     * \return Element.
     */
    [[nodiscard]] auto operator()(num_collect::index_type row,
        num_collect::index_type col) const -> scalar_type {
        if (row == col) {
            return static_cast<scalar_type>(1);
        }
        if (row + 1 == col) {
            return static_cast<scalar_type>(-1);
        }
        return static_cast<scalar_type>(0);
    }
};

}  // namespace impl

/*!
 * \brief Create a dense differential matrix.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] size Size (number of columns).
 * \return Expression.
 */
template <typename Matrix>
[[nodiscard]] inline auto dense_diff_matrix(num_collect::index_type size)
    -> Eigen::CwiseNullaryOp<impl::dense_diff_matrix_functor<Matrix>, Matrix> {
    NUM_COLLECT_ASSERT(size > 2);
    return Matrix::NullaryExpr(
        size - 1, size, impl::dense_diff_matrix_functor<Matrix>());
}

}  // namespace num_prob_collect::regularization
