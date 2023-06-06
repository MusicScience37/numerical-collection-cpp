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
 * \brief Definition of laplacian_2d_grid class.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include <Eigen/SparseCore>
#include <Eigen/src/SparseCore/SparseUtil.h>

#include "num_collect/base/concepts/sparse_matrix.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"

namespace num_prob_collect::finite_element {

/*!
 * \brief Class to create matrices of Laplacian on a 2-dimensional grid.
 *
 * \note Discretization is based on bilinear finite elements.
 *
 * \tparam Matrix Type of the matrix.
 */
template <num_collect::base::concepts::sparse_matrix Matrix>
class laplacian_2d_grid {
public:
    //! Type of the matrix.
    using matrix_type = Matrix;

    //! Type of the scalars.
    using scalar_type = typename Matrix::Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] grid_rows Number of rows in the grid.
     * \param[in] grid_cols Number of columns in the grid. (Excluding
     * boundaries.)
     * \param[in] grid_width Width of squares in the grid.
     */
    laplacian_2d_grid(num_collect::index_type grid_rows,
        num_collect::index_type grid_cols,
        typename Matrix::value_type grid_width)
        : grid_rows_(grid_rows),
          grid_cols_(grid_cols),
          mat_size_(grid_rows_ * grid_cols_),
          grid_width_(grid_width),
          mat_(mat_size_, mat_size_) {
        NUM_COLLECT_ASSERT(grid_rows > 0);
        NUM_COLLECT_ASSERT(grid_cols > 0);
        NUM_COLLECT_ASSERT(grid_width > static_cast<scalar_type>(0));
        calc();
    }

    /*!
     * \brief Get the number of rows in the grid.
     *
     * \return Number of rows in the grid.
     */
    [[nodiscard]] auto grid_rows() const noexcept -> num_collect::index_type {
        return grid_rows_;
    }

    /*!
     * \brief Get the number of columns in the grid.
     *
     * \return Number of columns in the grid.
     */
    [[nodiscard]] auto grid_cols() const noexcept -> num_collect::index_type {
        return grid_cols_;
    }

    /*!
     * \brief Get the size of the matrix.
     *
     * \return Size of the matrix.
     */
    [[nodiscard]] auto mat_size() const noexcept -> num_collect::index_type {
        return mat_size_;
    }

    /*!
     * \brief Get the coefficient of diagonal elements.
     *
     * \return Coefficient.
     */
    [[nodiscard]] auto diag_coeff() const noexcept -> scalar_type {
        return diag_coeff_;
    }

    /*!
     * \brief Get the coefficient of off-diagonal elements.
     *
     * \return Coefficient.
     */
    [[nodiscard]] auto off_diag_coeff() const noexcept -> scalar_type {
        return off_diag_coeff_;
    }

    /*!
     * \brief Get the Laplacian matrix.
     *
     * \return Laplacian matrix.
     */
    [[nodiscard]] auto mat() const noexcept -> const Matrix& { return mat_; }

    /*!
     * \brief Calculate an index in the matrix.
     *
     * \param[in] x Index of x in grid.
     * \param[in] y Index of y in grid.
     * \return Index in the matrix.
     */
    [[nodiscard]] auto index(num_collect::index_type x,
        num_collect::index_type y) const noexcept -> num_collect::index_type {
        return x + grid_cols_ * y;
    }

private:
    /*!
     * \brief Calculate matrices.
     */
    void calc() {
        const scalar_type inv_area =
            static_cast<scalar_type>(1) / (grid_width_ * grid_width_);
        // NOLINTNEXTLINE
        diag_coeff_ = static_cast<scalar_type>(8) /
            static_cast<scalar_type>(3) * inv_area;
        // NOLINTNEXTLINE
        off_diag_coeff_ = -inv_area / static_cast<scalar_type>(3);

        std::vector<Eigen::Triplet<scalar_type>> triplets;
        const std::size_t approx_elements =
            9U * static_cast<std::size_t>(mat_size_);
        triplets.reserve(approx_elements);
        for (num_collect::index_type xi = 0; xi < grid_cols_; ++xi) {
            for (num_collect::index_type yi = 0; yi < grid_rows_; ++yi) {
                const num_collect::index_type i = index(xi, yi);
                for (num_collect::index_type
                         xj = std::max<num_collect::index_type>(xi - 1, 0),
                         xj_end = std::min<num_collect::index_type>(
                             xi + 2, grid_cols_);
                     xj < xj_end; ++xj) {
                    for (num_collect::index_type
                             yj = std::max<num_collect::index_type>(yi - 1, 0),
                             yj_end = std::min<num_collect::index_type>(
                                 yi + 2, grid_rows_);
                         yj < yj_end; ++yj) {
                        const num_collect::index_type j = index(xj, yj);
                        const scalar_type coeff =
                            (i == j) ? diag_coeff_ : off_diag_coeff_;
                        triplets.emplace_back(
                            static_cast<int>(i), static_cast<int>(j), coeff);
                    }
                }
            }
        }

        mat_.setFromTriplets(triplets.begin(), triplets.end());
    }

    //! Number of rows in the grid.
    num_collect::index_type grid_rows_;

    //! Number of columns in the grid.
    num_collect::index_type grid_cols_;

    //! Number of rows in the Laplacian matrix.
    num_collect::index_type mat_size_;

    //! Width of squares in the grid.
    scalar_type grid_width_;

    //! Coefficient of diagonal elements.
    scalar_type diag_coeff_{};

    //! Coefficient of off-diagonal elements.
    scalar_type off_diag_coeff_{};

    //! Laplacian matrix.
    Matrix mat_;
};

}  // namespace num_prob_collect::finite_element
