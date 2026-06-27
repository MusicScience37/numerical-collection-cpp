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
        input_matrix_ = matrix_type::Zero(max_rows, max_cols);
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
    void append_column(const Eigen::EigenBase<Vector>& column) {
        NUM_COLLECT_PRECONDITION(
            column.cols() == 1, "Column must be a vector.");
        NUM_COLLECT_PRECONDITION(column.rows() >= rows_,
            "Additional column must have at least the same number of rows as "
            "the current input matrix.");
        NUM_COLLECT_PRECONDITION(column.rows() <= input_matrix_.rows(),
            "Additional column must have at most the maximum number of rows.");
        NUM_COLLECT_PRECONDITION(cols_ < input_matrix_.cols(),
            "Cannot append more columns than the maximum number of columns.");

        rows_ = column.rows();
        input_matrix_.col(cols_).head(rows_) = column;
        ++cols_;
        if (cols_ == 0) {
            handle_first_column();
        } else {
            handle_additional_column();
        }
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
     * This function performs the QR decomposition for the first column using
     * Givens rotations.
     */
    void handle_first_column() {
        r_.col(0).head(rows_) = input_matrix_.col(0).head(rows_);
        for (index_type i = rows_ - 1; i >= 1; --i) {
            const auto [c, s] =
                compute_givens_rotation_coefficients(r_(i - 1, 0), r_(i, 0));
            const Eigen::Matrix2d rotation_matrix{{c, s}, {-s, c}};
            r_.template block<2, 1>(i - 1, 0) =
                rotation_matrix.transpose() * r_.template block<2, 1>(i - 1, 0);
            q_.block(0, i - 1, rows_, 2) =
                q_.block(0, i - 1, rows_, 2) * rotation_matrix;
        }
    }

    /*!
     * \brief Handle the additional column of the input matrix.
     */
    void handle_additional_column() {
        r_.topLeftCorner(rows_, cols_) =
            q_.topLeftCorner(rows_, rows_).transpose() *
            input_matrix_.topLeftCorner(rows_, cols_);
        for (index_type i = rows_ - 1; i >= cols_; --i) {
            const auto [c, s] = compute_givens_rotation_coefficients(
                r_(i - 1, cols_ - 1), r_(i, cols_ - 1));
            const Eigen::Matrix2d rotation_matrix{{c, s}, {-s, c}};
            r_.template block<2, 1>(i - 1, cols_ - 1) =
                rotation_matrix.transpose() *
                r_.template block<2, 1>(i - 1, cols_ - 1);
            q_.block(0, i - 1, rows_, 2) =
                q_.block(0, i - 1, rows_, 2) * rotation_matrix;
        }
    }

    //! Matrix to decompose.
    matrix_type input_matrix_{};

    //! Matrix Q in QR decomposition.
    matrix_type q_{};

    //! Matrix R in QR decomposition.
    matrix_type r_{};

    //! Current number of columns in the input matrix.
    index_type cols_{};

    //! Current number of rows in the input matrix.
    index_type rows_{};
};

}  // namespace num_collect::linear::impl
