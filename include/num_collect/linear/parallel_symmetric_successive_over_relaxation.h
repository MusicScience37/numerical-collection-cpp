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
 * \brief Definition of parallel_symmetric_successive_over_relaxation class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <string_view>

#include <Eigen/Core>
#include <omp.h>

#include "num_collect/base/concepts/dense_vector_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar.h"      // IWYU pragma: keep
#include "num_collect/base/concepts/sparse_matrix.h"    // IWYU pragma: keep
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/iterative_solver_base.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/util/assert.h"

namespace num_collect::linear {

template <base::concepts::sparse_matrix Matrix>
class parallel_symmetric_successive_over_relaxation;

//! Log tag of parallel_symmetric_successive_over_relaxation.
constexpr auto parallel_symmetric_successive_over_relaxation_tag =
    logging::log_tag_view(
        "num_collect::linear::parallel_symmetric_successive_over_relaxation");

namespace impl {

/*!
 * \brief Traits of parallel_symmetric_successive_over_relaxation class.
 *
 * \tparam Matrix Type of the matrix.
 */
template <base::concepts::sparse_matrix Matrix>
struct iterative_solver_traits<
    parallel_symmetric_successive_over_relaxation<Matrix>> {
    //! Type of the matrix.
    using matrix_type = Matrix;
};

}  // namespace impl

/*!
 * \brief Class to solve linear equations using symmetric successive
 * over-relaxation using threads \cite Golub2013.
 *
 * \tparam Matrix Type of the matrix.
 */
template <base::concepts::sparse_matrix Matrix>
class parallel_symmetric_successive_over_relaxation
    : public iterative_solver_base<
          parallel_symmetric_successive_over_relaxation<Matrix>>,
      public logging::logging_mixin {
    static_assert(Matrix::IsRowMajor == 1, "Row major matrix is required.");
    static_assert(base::concepts::real_scalar<typename Matrix::Scalar>,
        "Complex matrices are not supported.");

public:
    //! Type of the base class.
    using base_type = iterative_solver_base<
        parallel_symmetric_successive_over_relaxation<Matrix>>;

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
    parallel_symmetric_successive_over_relaxation()
        : logging::logging_mixin(
              parallel_symmetric_successive_over_relaxation_tag) {}

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
        run_parallel_ =
            (coeff.nonZeros() / omp_get_max_threads() > 1000);  // NOLINT
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
            if (run_parallel_) {
                iterate_parallel(coeff_ref, right, solution);
            } else {
                iterate_no_parallel(coeff_ref, right, solution);
            }
            if (!std::isfinite(residual_)) {
                throw algorithm_failure(
                    "Failure in "
                    "parallel_symmetric_successive_over_relaxation.");
            }
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
        -> parallel_symmetric_successive_over_relaxation& {
        if (val <= static_cast<scalar_type>(0) ||
            static_cast<scalar_type>(2) <= val) {
            throw invalid_argument("Invalid relaxation coefficient.");
        }
        relaxation_coeff_ = val;
        return *this;
    }

    /*!
     * \brief Set whether to run in parallel.
     *
     * \note Default value is set in compute function.
     *
     * \param[in] val Whether to run in parallel.
     * \return This.
     */
    auto run_parallel(bool val)
        -> parallel_symmetric_successive_over_relaxation& {
        run_parallel_ = val;
        return *this;
    }

private:
    /*!
     * \brief Iterate once in parallel.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] coeff_ref Coefficient matrix.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void iterate_parallel(const matrix_type& coeff_ref, const Right& right,
        Solution& solution) const {
        const index_type size = coeff_ref.rows();
        const scalar_type prev_sol_coeff =
            static_cast<scalar_type>(1) - relaxation_coeff_;
        residual_ = static_cast<scalar_type>(0);

#pragma omp parallel
        {
            const index_type num_threads = omp_get_num_threads();
            const index_type thread_id = omp_get_thread_num();
            const index_type rows_per_thread =
                (size + num_threads - 1) / num_threads;
            const index_type my_start_row = thread_id * rows_per_thread;
            const index_type my_last_row =
                std::min(my_start_row + rows_per_thread, size);

            if (iterations_ == 0) {
                this->logger().trace()("my rows: {} - {} (whole rows: {})",
                    my_start_row, my_last_row, size);
            }

            auto my_residual = static_cast<scalar_type>(0);

            // Forward update.
            for (index_type i = my_start_row; i < my_last_row; ++i) {
                my_residual += process_row_forward(coeff_ref, right, solution,
                    i, my_start_row, prev_sol_coeff);
            }

#pragma omp barrier

            // Backward update.
            for (index_type i = my_last_row - 1; i >= my_start_row; --i) {
                process_row_backward(
                    coeff_ref, right, solution, i, my_last_row, prev_sol_coeff);
            }

#pragma omp critical
            residual_ += my_residual;
        }
    }

    /*!
     * \brief Iterate once in single thread.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] coeff_ref Coefficient matrix.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void iterate_no_parallel(const matrix_type& coeff_ref, const Right& right,
        Solution& solution) const {
        const index_type size = coeff_ref.rows();
        const scalar_type prev_sol_coeff =
            static_cast<scalar_type>(1) - relaxation_coeff_;
        residual_ = static_cast<scalar_type>(0);

        // Forward update.
        for (index_type i = 0; i < size; ++i) {
            residual_ += process_row_forward(
                coeff_ref, right, solution, i, 0, prev_sol_coeff);
        }

        // Backward update.
        for (index_type i = size - 1; i >= 0; --i) {
            process_row_backward(
                coeff_ref, right, solution, i, size, prev_sol_coeff);
        }
    }

    /*!
     * \brief Process a row in the forward update.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] coeff_ref Coefficient matrix.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     * \param[in] i Index of the row.
     * \param[in] start_row Index of the first row in this thread.
     * \param[in] prev_sol_coeff Coefficient for the previous solution.
     * \return Residual of this row.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    auto process_row_forward(const matrix_type& coeff_ref, const Right& right,
        Solution& solution, index_type i, index_type start_row,
        const scalar_type& prev_sol_coeff) const -> scalar_type {
        scalar_type numerator = right(i);
        for (typename matrix_type::InnerIterator iter(coeff_ref, i); iter;
             ++iter) {
            if (start_row <= iter.index() && iter.index() < i) {
                numerator -=
                    iter.value() * intermidiate_solution_(iter.index());
            } else if (iter.index() != i) {
                numerator -= iter.value() * solution(iter.index());
            }
        }
        const scalar_type row_residual = numerator - diag_(i) * solution(i);
        intermidiate_solution_(i) =
            relaxation_coeff_ * numerator * inv_diag_(i) +
            prev_sol_coeff * solution(i);
        return row_residual * row_residual;
    }

    /*!
     * \brief Process a row in the backward update.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] coeff_ref Coefficient matrix.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     * \param[in] i Index of the row.
     * \param[in] end_row Index of the pat-the-last row in this thread.
     * \param[in] prev_sol_coeff Coefficient for the previous solution.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void process_row_backward(const matrix_type& coeff_ref, const Right& right,
        Solution& solution, index_type i, index_type end_row,
        const scalar_type& prev_sol_coeff) const {
        scalar_type numerator = right(i);
        for (typename matrix_type::InnerIterator iter(coeff_ref, i); iter;
             ++iter) {
            if (i < iter.index() && iter.index() < end_row) {
                numerator -= iter.value() * solution(iter.index());
            } else if (iter.index() != i) {
                numerator -=
                    iter.value() * intermidiate_solution_(iter.index());
            }
        }
        solution(i) = relaxation_coeff_ * numerator * inv_diag_(i) +
            prev_sol_coeff * intermidiate_solution_(i);
    }

    //! Whether to run in parallel.
    bool run_parallel_{true};

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
