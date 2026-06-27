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
 * \brief Definition of incremental_qr class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <utility>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"

namespace num_collect::linear::impl {

/*!
 * \brief Compute the coefficients of the matrix of Givens rotation on a 2D
 * vector \cite Golub2013.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] a First element of the input vector.
 * \param[in] b Second element of the input vector.
 * \return Pair of two coefficients of the Givens rotation matrix.
 *
 * This function computes the coefficients \f$c = \cos\theta\f$ and
 * \f$s = \sin\theta\f$ which satisfy the following
 * equation:
 * \f[
 * \begin{pmatrix}
 *   c & s \\
 *   -s & c
 * \end{pmatrix}^T
 * \begin{pmatrix}
 *   a \\
 *   b
 * \end{pmatrix}=
 * \begin{pmatrix}
 *   r \\
 *   0
 * \end{pmatrix}
 * \f]
 */
template <typename Scalar>
[[nodiscard]] auto compute_givens_rotation_coefficients(
    const Scalar& a, const Scalar& b) -> std::pair<Scalar, Scalar> {
    using std::abs;
    if (abs(b) <= abs(a) * std::numeric_limits<Scalar>::epsilon()) {
        return {static_cast<Scalar>(1), static_cast<Scalar>(0)};
    }
    if (abs(b) > abs(a)) {
        const Scalar t = -a / b;
        const Scalar s =
            static_cast<Scalar>(1) / std::sqrt(static_cast<Scalar>(1) + t * t);
        const Scalar c = s * t;
        return {c, s};
    }
    // abs(a) >= abs(b)
    const Scalar t = -b / a;
    const Scalar c =
        static_cast<Scalar>(1) / std::sqrt(static_cast<Scalar>(1) + t * t);
    const Scalar s = c * t;
    return {c, s};
}

/*!
 * \brief Class to perform QR decomposition with incremental updates
 * \cite Golub2013.
 *
 * \tparam Scalar Type of scalars.
 *
 * \note This class is designed for internal use in
 * num_collect::linear::functional_gmres.
 * So this class works only when the number of rows is equal to the number of
 * columns plus one.
 * \note When the input matrix does not have full column rank,
 * this class uses only the columns that can be usable in solving linear
 * systems.
 */
template <typename Scalar>
class incremental_qr {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Type of matrices.
    using matrix_type = Eigen::MatrixX<scalar_type>;

    /*!
     * \brief Constructor.
     */
    incremental_qr() = default;

    /*!
     * \brief Initialize the internal state.
     *
     * \param[in] max_rows Maximum number of rows in the input matrix.
     * \param[in] max_cols Maximum number of columns in the input matrix.
     */
    void initialize(index_type max_rows, index_type max_cols) {
        NUM_COLLECT_PRECONDITION(max_rows > 0, "max_rows must be positive.");
        NUM_COLLECT_PRECONDITION(max_cols > 0, "max_cols must be positive.");
        NUM_COLLECT_PRECONDITION(max_rows == max_cols + 1,
            "Current implementation supports only max_rows = max_cols + 1.");

        q_ = matrix_type::Identity(max_rows, max_rows);
        r_ = matrix_type::Zero(max_rows, max_cols);
        cols_ = 0;
        rows_ = 0;
    }

    /*!
     * \brief Append a column to the input matrix and update the QR
     * decomposition.
     *
     * \tparam Vector Type of the vector to append.
     * \param[in] column Column to append.
     *
     * \note This function assumes that \ref initialize has been called before
     * this function.
     */
    template <typename Vector>
    void append_column(const Eigen::DenseBase<Vector>& column) {
        NUM_COLLECT_PRECONDITION(
            column.cols() == 1, "Column must be a vector.");
        NUM_COLLECT_PRECONDITION(column.rows() >= rows_,
            "Additional column must have at least the same number of rows as "
            "the current input matrix.");
        NUM_COLLECT_PRECONDITION(column.rows() <= r_.rows(),
            "Additional column must have at most the maximum number of rows.");
        NUM_COLLECT_PRECONDITION(cols_ < r_.cols(),
            "Cannot append more columns than the maximum number of columns.");

        rows_ = column.rows();
        ++cols_;
        if (cols_ == 1) {
            handle_first_column(column);
        } else {
            handle_additional_column(column);
        }
    }

    /*!
     * \brief Solve a linear system using the QR decomposition.
     *
     * \tparam RhsVector Type of the right-hand-side vector.
     * \tparam SolutionVector Type of the solution vector.
     * \param[in] rhs Right-hand-side vector.
     * \param[out] solution Solution vector.
     */
    template <typename RhsVector, typename SolutionVector>
    void solve(const Eigen::DenseBase<RhsVector>& rhs,
        const Eigen::DenseBase<SolutionVector>& solution) {
        NUM_COLLECT_PRECONDITION(
            rhs.cols() == 1, "Right-hand-side must be a vector.");
        NUM_COLLECT_PRECONDITION(rhs.rows() == rows_,
            "Right-hand-side must have the same number of rows as the input "
            "matrix.");
        NUM_COLLECT_PRECONDITION(
            solution.cols() == 1, "Solution must be a vector.");

        auto& solution_derived =
            // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
            const_cast<Eigen::DenseBase<SolutionVector>&>(solution).derived();

        const index_type usable_cols = determine_usable_cols();
        if (usable_cols < cols_) {
            solution_derived = Eigen::VectorX<scalar_type>::Zero(cols_);
            if (usable_cols == 0) {
                return;
            }
            rhs_buffer_ = q_.topLeftCorner(rows_, usable_cols).transpose() *
                rhs.derived();
            solution_derived.head(usable_cols) =
                r_.topLeftCorner(usable_cols, usable_cols)
                    .template triangularView<Eigen::Upper>()
                    .solve(rhs_buffer_);
            return;
        }

        rhs_buffer_ =
            q_.topLeftCorner(rows_, cols_).transpose() * rhs.derived();
        solution_derived = r_.topLeftCorner(cols_, cols_)
                               .template triangularView<Eigen::Upper>()
                               .solve(rhs_buffer_);
    }

    /*!
     * \brief Estimate the residual norm when solving a linear system using the
     * QR decomposition.
     *
     * \tparam RhsVector Type of the right-hand-side vector.
     * \param[in] rhs Right-hand-side vector.
     * \return Estimated residual norm.
     */
    template <typename RhsVector>
    [[nodiscard]] auto estimate_residual_norm(
        const Eigen::DenseBase<RhsVector>& rhs) -> scalar_type {
        NUM_COLLECT_PRECONDITION(
            rhs.cols() == 1, "Right-hand-side must be a vector.");
        NUM_COLLECT_PRECONDITION(rhs.rows() == rows_,
            "Right-hand-side must have the same number of rows as the input "
            "matrix.");

        const index_type usable_cols = determine_usable_cols();
        rhs_buffer_ =
            q_.topLeftCorner(rows_, rows_).transpose() * rhs.derived();
        const scalar_type residual_norm =
            rhs_buffer_.tail(rows_ - usable_cols).stableNorm();
        return residual_norm;
    }

    /*!
     * \brief Get the matrix Q in the QR decomposition.
     *
     * \return Matrix Q.
     */
    [[nodiscard]] auto q() const { return q_.topLeftCorner(rows_, rows_); }

    /*!
     * \brief Get the matrix R in the QR decomposition.
     *
     * \return Matrix R.
     */
    [[nodiscard]] auto r() const { return r_.topLeftCorner(rows_, cols_); }

private:
    /*!
     * \brief Handle the first column of the input matrix.
     *
     * \tparam Vector Type of the vector to append.
     * \param[in] column First column of the input matrix.
     *
     * This function performs the QR decomposition for the first column using
     * Givens rotations.
     */
    template <typename Vector>
    void handle_first_column(const Eigen::DenseBase<Vector>& column) {
        r_.col(0).head(rows_) = column.derived();
        for (index_type i = rows_ - 1; i >= 1; --i) {
            const auto [c, s] =
                compute_givens_rotation_coefficients(r_(i - 1, 0), r_(i, 0));
            const Eigen::Matrix2<scalar_type> rotation_matrix{{c, s}, {-s, c}};
            r_.template block<2, 1>(i - 1, 0) =
                rotation_matrix.transpose() * r_.template block<2, 1>(i - 1, 0);
            q_.block(0, i - 1, rows_, 2) =
                q_.block(0, i - 1, rows_, 2) * rotation_matrix;
        }
    }

    /*!
     * \brief Handle the additional column of the input matrix.
     *
     * \tparam Vector Type of the vector to append.
     * \param[in] column Column of the input matrix to append.
     */
    template <typename Vector>
    void handle_additional_column(const Eigen::DenseBase<Vector>& column) {
        r_.col(cols_ - 1).head(rows_) =
            q_.topLeftCorner(rows_, rows_).transpose() * column.derived();
        for (index_type i = rows_ - 1; i >= cols_; --i) {
            const auto [c, s] = compute_givens_rotation_coefficients(
                r_(i - 1, cols_ - 1), r_(i, cols_ - 1));
            const Eigen::Matrix2<scalar_type> rotation_matrix{{c, s}, {-s, c}};
            r_.template block<2, 1>(i - 1, cols_ - 1) =
                rotation_matrix.transpose() *
                r_.template block<2, 1>(i - 1, cols_ - 1);
            q_.block(0, i - 1, rows_, 2) =
                q_.block(0, i - 1, rows_, 2) * rotation_matrix;
        }
    }

    /*!
     * \brief Calculate the number of columns usable in solving linear systems.
     *
     * \return Number of columns usable in solving linear systems.
     */
    [[nodiscard]] auto determine_usable_cols() const -> index_type {
        const index_type min_dim = std::min(rows_, cols_);
        const scalar_type max_diag =
            r_.topLeftCorner(min_dim, min_dim).diagonal().cwiseAbs().maxCoeff();
        const scalar_type diag_thresh =
            std::numeric_limits<scalar_type>::epsilon() * max_diag;
        for (index_type i = 0; i < min_dim; ++i) {
            if (std::abs(r_(i, i)) <= diag_thresh) {
                return i;
            }
        }
        return min_dim;
    }

    //! Matrix Q in QR decomposition.
    matrix_type q_{};

    //! Matrix R in QR decomposition.
    matrix_type r_{};

    //! Current number of columns in the input matrix.
    index_type cols_{};

    //! Current number of rows in the input matrix.
    index_type rows_{};

    //! Buffer for the right-hand-side vector in solving linear systems.
    Eigen::VectorX<scalar_type> rhs_buffer_{};
};

}  // namespace num_collect::linear::impl
