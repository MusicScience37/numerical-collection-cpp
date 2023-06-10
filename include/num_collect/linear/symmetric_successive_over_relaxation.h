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
 * \brief Definition of symmetric_successive_over_relaxation class.
 */
#pragma once

#include <cmath>

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar.h"      // IWYU pragma: keep
#include "num_collect/base/concepts/sparse_matrix.h"    // IWYU pragma: keep
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/iterative_solver_base.h"
#include "num_collect/util/assert.h"

namespace num_collect::linear {

template <base::concepts::sparse_matrix Matrix>
class symmetric_successive_over_relaxation;

namespace impl {

/*!
 * \brief Traits of gauss_seidel_iterative_solver class.
 *
 * \tparam Matrix Type of the matrix.
 */
template <base::concepts::sparse_matrix Matrix>
struct iterative_solver_traits<symmetric_successive_over_relaxation<Matrix>> {
    //! Type of the matrix.
    using matrix_type = Matrix;
};

}  // namespace impl

/*!
 * \brief Class to solve linear equations using symmetric successive
 * over-relaxation \cite Golub2013.
 *
 * \tparam Matrix Type of the matrix.
 */
template <base::concepts::sparse_matrix Matrix>
class symmetric_successive_over_relaxation
    : public iterative_solver_base<
          symmetric_successive_over_relaxation<Matrix>> {
    static_assert(Matrix::IsRowMajor == 1, "Row major matrix is required.");
    static_assert(base::concepts::real_scalar<typename Matrix::Scalar>,
        "Complex matrices are not supported.");

public:
    //! Type of the base class.
    using base_type =
        iterative_solver_base<symmetric_successive_over_relaxation<Matrix>>;

    using typename base_type::matrix_type;
    using typename base_type::real_scalar_type;
    using typename base_type::scalar_type;
    using typename base_type::storage_index_type;

protected:
    using base_type::coeff;

public:
    //! Type of vectors.
    using vector_type = Eigen::VectorX<scalar_type>;

    /*!
     * \brief Constructor.
     */
    symmetric_successive_over_relaxation() = default;

    /*!
     * \brief Prepare to solve.
     *
     * \param[in] coeff Coefficient matrix.
     */
    void compute(const matrix_type& coeff) {
        base_type::compute(coeff);
        diag_ = coeff.diagonal();
        inv_diag_ = diag_.cwiseInverse();
        intermidiate_solution_.resize(coeff.cols());
        if (!inv_diag_.array().isFinite().all()) {
            throw invalid_argument(
                "All diagonal elements of the coefficient matrix must not be "
                "zero.");
        }
    }

    /*!
     * \brief Iterate repeatedly until stop criterion is satisfied for a vector.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void solve_vector_in_place(const Right& right, Solution& solution) const {
        const auto& coeff_ref = coeff();

        NUM_COLLECT_ASSERT(coeff_ref.rows() == coeff_ref.cols());
        NUM_COLLECT_ASSERT(right.size() == coeff_ref.cols());
        NUM_COLLECT_ASSERT(solution.size() == coeff_ref.cols());

        iterations_ = 0;
        const scalar_type right_norm = right.squaredNorm();
        const index_type max_iterations = base_type::max_iterations();
        while (iterations_ < max_iterations) {
            iterate(coeff_ref, right, solution);
            ++iterations_;
            using std::sqrt;
            residual_rate_ = sqrt(residual_ / right_norm);
            if (residual_rate_ < base_type::tolerance()) {
                break;
            }
        }
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \note This value won't be updated in iterate function.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Get the rate of the last residual.
     *
     * \note This value won't be updated in iterate function.
     *
     * \return Rate of the last residual.
     */
    [[nodiscard]] auto residual_rate() const noexcept -> index_type {
        return residual_rate_;
    }

    /*!
     * \brief Set the relaxation coefficient.
     *
     * \param[in] val Relaxation coefficient.
     * \return This.
     */
    auto relaxation_coeff(const scalar_type& val)
        -> symmetric_successive_over_relaxation& {
        if (val <= static_cast<scalar_type>(0) ||
            static_cast<scalar_type>(2) <= val) {
            throw invalid_argument("Invalid relaxation coefficient.");
        }
        relaxation_coeff_ = val;
        return *this;
    }

private:
    /*!
     * \brief Iterate once.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] coeff_ref Coefficient matrix.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void iterate(const matrix_type& coeff_ref, const Right& right,
        Solution& solution) const {
        const index_type size = coeff_ref.rows();
        const scalar_type prev_sol_coeff =
            static_cast<scalar_type>(1) - relaxation_coeff_;
        residual_ = static_cast<scalar_type>(0);

        // Forward update.
        for (index_type i = 0; i < size; ++i) {
            scalar_type numerator = right(i);
            for (typename matrix_type::InnerIterator iter(coeff_ref, i); iter;
                 ++iter) {
                if (iter.index() < i) {
                    numerator -=
                        iter.value() * intermidiate_solution_(iter.index());
                } else if (iter.index() > i) {
                    numerator -= iter.value() * solution(iter.index());
                }
            }
            const scalar_type row_residual = numerator - diag_(i) * solution(i);
            intermidiate_solution_(i) =
                relaxation_coeff_ * numerator * inv_diag_(i) +
                prev_sol_coeff * solution(i);
            residual_ += row_residual * row_residual;
        }

        // Backward update.
        for (index_type i = size - 1; i >= 0; --i) {
            scalar_type numerator = right(i);
            for (typename matrix_type::InnerIterator iter(coeff_ref, i); iter;
                 ++iter) {
                if (iter.index() < i) {
                    numerator -=
                        iter.value() * intermidiate_solution_(iter.index());
                } else if (iter.index() > i) {
                    numerator -= iter.value() * solution(iter.index());
                }
            }
            solution(i) = relaxation_coeff_ * numerator * inv_diag_(i) +
                prev_sol_coeff * intermidiate_solution_(i);
        }
    }

    //! Number of iterations.
    mutable index_type iterations_{};

    //! Last residual.
    mutable scalar_type residual_{};

    //! Rate of last residual.
    mutable scalar_type residual_rate_{};

    //! Relaxation coefficient.
    scalar_type relaxation_coeff_{static_cast<scalar_type>(1)};

    //! Diagonal coefficients.
    vector_type diag_{};

    //! Inverse of diagonal coefficients.
    vector_type inv_diag_{};

    //! Intermidiate solution vector.
    mutable vector_type intermidiate_solution_{};
};

}  // namespace num_collect::linear
