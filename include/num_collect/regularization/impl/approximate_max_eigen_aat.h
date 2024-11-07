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
 * \brief Definition of max_eigen_aat function.
 */
#pragma once

#include <cmath>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar_dense_matrix.h"
#include "num_collect/base/concepts/real_scalar_sparse_matrix.h"
#include "num_collect/base/index_type.h"

namespace num_collect::regularization::impl {

/*!
 * \brief Approximate the maximum eigenvalue of \f$ AA^T \f$
 * for a matrix \f$ A \f$.
 *
 * \param[in] matrix Matrix.
 * \return Eigenvalue.
 */
template <typename Matrix>
    requires(base::concepts::real_scalar_dense_matrix<Matrix> ||
        base::concepts::real_scalar_sparse_matrix<Matrix>)
[[nodiscard]] auto approximate_max_eigen_aat(const Matrix& matrix) ->
    typename Matrix::Scalar {
    const index_type rows = matrix.rows();
    using scalar_type = typename Matrix::Scalar;
    using vector_type = Eigen::VectorX<scalar_type>;
    vector_type vec = vector_type::Random(rows);
    vec.normalize();

    vector_type mul_vec = matrix * matrix.transpose() * vec;
    scalar_type eigen = vec.dot(mul_vec) / vec.squaredNorm();
    const index_type num_iterations = rows * 10;
    for (index_type i = 0; i < num_iterations; ++i) {
        const scalar_type eigen_before = eigen;
        vec = mul_vec.normalized();
        mul_vec = matrix * matrix.transpose() * vec;
        eigen = vec.dot(mul_vec) / vec.squaredNorm();
        using std::abs;
        constexpr auto tol_update = static_cast<scalar_type>(1e-4);
        if (abs(eigen - eigen_before) / abs(eigen) < tol_update) {
            break;
        }
    }

    return eigen;
}

}  // namespace num_collect::regularization::impl
