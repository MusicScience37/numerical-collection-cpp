/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of functional_gmres class.
 */
#pragma once

// IWYU pragma: no_include <Eigen/SparseCore>

#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>

#include <Eigen/Core>
#include <Eigen/QR>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"

namespace num_collect::linear {

//! Log tag.
constexpr auto functional_gmres_tag =
    logging::log_tag_view("num_collect::linear::functional_gmres");

/*!
 * \brief Class to solve linear equations using restarted generalized minimal
 * residual (restarted GMRES) \cite Golub2013 using functions to multiply
 * coefficient matrix.
 *
 * \tparam Vector Type of vectors.
 *
 * \note This class in the default setting won't restart GMRES because
 * restarting GMRES in the current simple implementation doesn't work well.
 */
template <base::concepts::real_scalar_dense_vector Vector>
class functional_gmres : public logging::logging_mixin {
public:
    //! Type of vectors.
    using vector_type = Vector;

    //! Type of scalars.
    using scalar_type = typename vector_type::Scalar;

    //! Constructor.
    functional_gmres() : logging::logging_mixin(functional_gmres_tag) {}

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
    template <std::invocable<const vector_type&, vector_type&> CoeffFunction>
    void solve(CoeffFunction&& coeff_function, const vector_type& rhs,
        vector_type& solution) {
        const index_type max_subspace_dim =
            std::min(max_subspace_dim_, solution.size());

        // Initialize memory.
        basis_vector_buffer_.resize(solution.size());
        basis_.resize(solution.size(), max_subspace_dim);
        hessenberg_.resize(max_subspace_dim + 1, max_subspace_dim);
        temp_rhs_.resize(max_subspace_dim + 1);
        temp_sol_.resize(max_subspace_dim);
        temp_vec_q_.resize(max_subspace_dim + 1);

        // temp_rhs_'s elements are zero except for the first element, which
        // will be set in iterations.
        temp_rhs_.setZero();

        iterations_ = 0;

        // Vectors below this threshold may not work in dot products.
        const scalar_type basis_vector_norm_thresh =
            std::sqrt(std::numeric_limits<scalar_type>::epsilon());

        const scalar_type rhs_norm = rhs.stableNorm();
        const scalar_type absolute_tolerance =
            std::max(rhs_norm * tolerance_, basis_vector_norm_thresh);

        coeff_function(solution, basis_vector_buffer_);
        basis_vector_buffer_ = rhs - basis_vector_buffer_;
        residual_norm_ = basis_vector_buffer_.stableNorm();
        NUM_COLLECT_LOG_TRACE(
            this->logger(), "First residual_norm={}", residual_norm_);
        while (residual_norm_ > absolute_tolerance &&
            iterations_ < max_iterations_) {
            hessenberg_.setZero();

            // First subspace in GMRES.
            index_type current_subspace_dim = 0;
            temp_rhs_(0) = residual_norm_;
            scalar_type basis_vector_norm = residual_norm_;
            while (residual_norm_ > absolute_tolerance &&
                basis_vector_norm > basis_vector_norm_thresh &&
                current_subspace_dim < max_subspace_dim &&
                iterations_ < max_iterations_) {
                // An iteration in GMRES.
                normalized_basis_vector_ =
                    basis_vector_buffer_ / basis_vector_norm;
                basis_.col(current_subspace_dim) = normalized_basis_vector_;
                ++current_subspace_dim;
                coeff_function(normalized_basis_vector_, basis_vector_buffer_);
                for (index_type i = 0; i < current_subspace_dim; ++i) {
                    scalar_type& current_coeff =
                        hessenberg_(i, current_subspace_dim - 1);
                    current_coeff = basis_.col(i).dot(basis_vector_buffer_);
                    basis_vector_buffer_ -= current_coeff * basis_.col(i);
                }
                basis_vector_norm = basis_vector_buffer_.stableNorm();
                hessenberg_(current_subspace_dim, current_subspace_dim - 1) =
                    basis_vector_norm;

                // Compute the current residual norm.
                // TODO Update QR decomposition incrementally.
                qr_.compute(hessenberg_.topLeftCorner(
                    current_subspace_dim + 1, current_subspace_dim));
                temp_vec_q_ = qr_.householderQ().adjoint() *
                    temp_rhs_.head(current_subspace_dim + 1);
                residual_norm_ = std::abs(temp_vec_q_(current_subspace_dim));

                ++iterations_;
                NUM_COLLECT_LOG_TRACE(this->logger(),
                    "iterations={}, current_subspace_dim={}, residual_norm={}",
                    iterations_, current_subspace_dim, residual_norm_);
            }
            if (current_subspace_dim == 0) {
                break;
            }

            temp_sol_.head(current_subspace_dim) =
                qr_.solve(temp_rhs_.head(current_subspace_dim + 1));
            solution += basis_.leftCols(current_subspace_dim) *
                temp_sol_.head(current_subspace_dim);

            // Prepare for the next GMRES iterations.
            // TODO Better restarting method.
            coeff_function(solution, basis_vector_buffer_);
            basis_vector_buffer_ = rhs - basis_vector_buffer_;
            residual_norm_ = basis_vector_buffer_.stableNorm();
            NUM_COLLECT_LOG_TRACE(this->logger(),
                "Finished one cycle of GMRES: iterations={}, residual_norm={}",
                iterations_, residual_norm_);
        }

        NUM_COLLECT_LOG_TRACE(this->logger(),
            "Finished restarted GMRES: iterations={}, residual_norm={}",
            iterations_, residual_norm_);
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_iterations(index_type val) -> functional_gmres& {
        NUM_COLLECT_PRECONDITION(val > 0, "max_iterations must be positive.");
        max_iterations_ = val;
        return *this;
    }

    /*!
     * \brief Set the maximum number of dimensions of subspace.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_subspace_dim(index_type val) -> functional_gmres& {
        NUM_COLLECT_PRECONDITION(val > 0,
            "Maximum number of dimensions of subspace must be a positive "
            "integer.");
        max_subspace_dim_ = val;
        return *this;
    }

    /*!
     * \brief Set the relative tolerance.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerance(scalar_type val) -> functional_gmres& {
        NUM_COLLECT_PRECONDITION(val <= static_cast<scalar_type>(1),
            "tolerance must be less than or equal to 1.");
        NUM_COLLECT_PRECONDITION(
            val >= std::numeric_limits<scalar_type>::epsilon(),
            "tolerance must be greater than or equal to machine epsilon.");
        tolerance_ = val;
        return *this;
    }

private:
    //! Type of internal matrices.
    using matrix_type = Eigen::MatrixX<scalar_type>;

    //! Buffer of basis vector for computation.
    vector_type basis_vector_buffer_{};

    //! Normalized basis vector.
    vector_type normalized_basis_vector_{};

    //! Basis of subspace.
    matrix_type basis_{};

    //! Hessenberg matrix.
    matrix_type hessenberg_{};

    //! Temporary right-hand-side vector for solving internal equation.
    Eigen::VectorX<scalar_type> temp_rhs_{};

    //! Temporary solution vector for solving internal equation.
    Eigen::VectorX<scalar_type> temp_sol_{};

    //! Temporary vector in the space of QR decomposition for solving internal equation.
    Eigen::VectorX<scalar_type> temp_vec_q_{};

    //! QR decomposition.
    Eigen::ColPivHouseholderQR<matrix_type> qr_{};

    //! Number of iterations.
    index_type iterations_{};

    //! Norm of the residual.
    scalar_type residual_norm_{};

    /*!
     * \brief Default maximum number of iterations.
     *
     * Default setting won't restart GMRES.
     */
    static constexpr index_type default_max_iterations = 100;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default maximum number of dimensions of subspace.
    static constexpr index_type default_max_subspace_dim = 100;

    //! Maximum number of dimensions of subspace.
    index_type max_subspace_dim_{default_max_subspace_dim};

    //! Relative tolerance.
    scalar_type tolerance_{Eigen::NumTraits<scalar_type>::dummy_precision()};
};

}  // namespace num_collect::linear
