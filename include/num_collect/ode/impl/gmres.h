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
 * \brief Definition of gmres class.
 */
#pragma once

// IWYU pragma: no_include <utility>
// IWYU pragma: no_include <Eigen/SparseCore>

#include <limits>

#include <Eigen/Core>
#include <Eigen/QR>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::ode::impl {

/*!
 * \brief Class to solve linear equations using generalized minimal residual
 * (GMRES) \cite Golub2013.
 *
 * \tparam Vector Type of vectors.
 */
template <base::concepts::real_scalar_dense_vector Vector>
class gmres {
public:
    //! Type of vectors.
    using vector_type = Vector;

    //! Type of scalars.
    using scalar_type = typename vector_type::Scalar;

    //! Type of matrices.
    using matrix_type = Eigen::MatrixX<scalar_type>;

    /*!
     * \brief Solve.
     *
     * coeff_function is assumed to have a signature like:
     *
     * ~~~{.cpp}
     * void coeff_function(const variable_type& target, variable_type& result);
     * ~~~
     *
     * \tparam CoeffFunction Type of the function to multiply coefficient
     * matrix.
     * \param[in] coeff_function Function to multiply coefficient matrix.
     * \param[in] rhs Right-hand-side vector.
     * \param[in,out] solution Solution. (Given vector is used as the initial
     * solution.)
     */
    template <typename CoeffFunction>
    void solve(CoeffFunction&& coeff_function, const vector_type& rhs,
        vector_type& solution) {
        if (max_subspace_dim_ > solution.size()) {
            max_subspace_dim_ = solution.size();
        }

        // Initialize matrices.
        basis_.resize(solution.size(), max_subspace_dim_);
        hessenberg_.resize(max_subspace_dim_ + 1, max_subspace_dim_);
        hessenberg_.setZero();
        temp_rhs_.resize(max_subspace_dim_ + 1);
        temp_rhs_.setZero();
        temp_sol_.resize(max_subspace_dim_);

        const scalar_type residual_thresh =
            rhs.norm() * std::numeric_limits<scalar_type>::epsilon();

        index_type k = 0;
        coeff_function(solution, residual_);
        residual_ = rhs - residual_;
        const scalar_type initial_residual_norm = residual_.norm();
        scalar_type residual_norm = initial_residual_norm;
        while (residual_norm > residual_thresh && k < max_subspace_dim_) {
            basis_.col(k) = residual_ / residual_norm;
            ++k;
            coeff_function(basis_.col(k - 1), residual_);
            for (index_type i = 0; i < k; ++i) {
                scalar_type& current_coeff = hessenberg_(i, k - 1);
                current_coeff = basis_.col(i).dot(residual_);
                residual_ -= current_coeff * basis_.col(i);
            }
            residual_norm = residual_.norm();
            hessenberg_(k, k - 1) = residual_norm;
        }
        if (k == 0) {
            return;
        }
        qr_.compute(hessenberg_.topLeftCorner(k + 1, k));
        temp_rhs_(0) = initial_residual_norm;
        temp_sol_.head(k) = qr_.solve(temp_rhs_.head(k + 1));
        solution += basis_.leftCols(k) * temp_sol_.head(k);
    }

    /*!
     * \brief Set the maximum number of dimensions of subspace.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_subspace_dim(index_type val) -> gmres& {
        if (val <= 0) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Maximum number of dimensions of subspace must be a positive "
                "integer.");
        }
        max_subspace_dim_ = val;
        return *this;
    }

private:
    //! Default maximum number of dimensions of subspace.
    static constexpr index_type default_max_subspace_dim = 2;

    //! Maximum number of dimensions of subspace.
    index_type max_subspace_dim_{default_max_subspace_dim};

    //! Residual.
    vector_type residual_{};

    //! Basis of subspace.
    matrix_type basis_{};

    //! Hessenberg matrix.
    matrix_type hessenberg_{};

    //! Temporary vector for solving internal equation.
    Eigen::VectorX<scalar_type> temp_rhs_{};

    //! Temporary vector for solving internal equation.
    Eigen::VectorX<scalar_type> temp_sol_{};

    //! QR decomposition.
    Eigen::ColPivHouseholderQR<matrix_type> qr_{};
};

}  // namespace num_collect::ode::impl
