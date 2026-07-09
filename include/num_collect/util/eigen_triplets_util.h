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
 * \brief Definition of utility functions for Eigen::Triplet.
 */
#pragma once

#include <ranges>
#include <type_traits>
#include <utility>

#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/impl/range_adaptor_closure.h"
#include "num_collect/util/vector.h"

//! Namespace of utility functions for Eigen::Triplet.
namespace num_collect::util::eigen_triplets {

/*!
 * \brief Create a range adaptor that shifts row indices of Eigen::Triplet.
 *
 * \param[in] row_offset The offset to add to the row indices.
 * \return A range adaptor that shifts row indices.
 *
 * \note This function does not check for overflow of the indices.
 */
[[nodiscard]] inline auto shift_rows(index_type row_offset) {
    return std::views::transform([row_offset](auto&& triplet) {
        using triplet_type = std::decay_t<decltype(triplet)>;
        using storage_index_type = std::decay_t<decltype(triplet.row())>;
        return triplet_type(
            static_cast<storage_index_type>(triplet.row() + row_offset),
            triplet.col(), triplet.value());
    });
}

/*!
 * \brief Create a range adaptor that shifts column indices of Eigen::Triplet.
 *
 * \param[in] column_offset The offset to add to the column indices.
 * \return A range adaptor that shifts column indices.
 *
 * \note This function does not check for overflow of the indices.
 */
[[nodiscard]] inline auto shift_columns(index_type column_offset) {
    return std::views::transform([column_offset](auto&& triplet) {
        using triplet_type = std::decay_t<decltype(triplet)>;
        using storage_index_type = std::decay_t<decltype(triplet.col())>;
        return triplet_type(triplet.row(),
            static_cast<storage_index_type>(triplet.col() + column_offset),
            triplet.value());
    });
}

/*!
 * \brief Create a range adaptor that shifts row and column indices of
 * Eigen::Triplet.
 *
 * \param[in] row_offset The offset to add to the row indices.
 * \param[in] column_offset The offset to add to the column indices.
 * \return A range adaptor that shifts row and column indices.
 *
 * \note This function does not check for overflow of the indices.
 */
[[nodiscard]] inline auto shift_rows_and_columns(
    index_type row_offset, index_type column_offset) {
    return std::views::transform([row_offset, column_offset](auto&& triplet) {
        using triplet_type = std::decay_t<decltype(triplet)>;
        using storage_index_type = std::decay_t<decltype(triplet.row())>;
        return triplet_type(
            static_cast<storage_index_type>(triplet.row() + row_offset),
            static_cast<storage_index_type>(triplet.col() + column_offset),
            triplet.value());
    });
}

/*!
 * \brief Create a range adaptor that filters Eigen::Triplet with row indices in
 * a specified range.
 *
 * \param[in] row_start The start of the row index range (inclusive).
 * \param[in] row_end The end of the row index range (exclusive).
 * \return A range adaptor that filters Eigen::Triplet by row indices.
 */
[[nodiscard]] inline auto filter_rows(
    index_type row_start, index_type row_end) {
    return std::views::filter([row_start, row_end](auto&& triplet) {
        return triplet.row() >= row_start && triplet.row() < row_end;
    });
}

/*!
 * \brief Create a range adaptor that filters Eigen::Triplet with column indices
 * in a specified range.
 *
 * \param[in] column_start The start of the column index range (inclusive).
 * \param[in] column_end The end of the column index range (exclusive).
 * \return A range adaptor that filters Eigen::Triplet by column indices.
 */
[[nodiscard]] inline auto filter_columns(
    index_type column_start, index_type column_end) {
    return std::views::filter([column_start, column_end](auto&& triplet) {
        return triplet.col() >= column_start && triplet.col() < column_end;
    });
}

/*!
 * \brief Convert a range of Eigen::Triplet to a vector of Eigen::Triplet.
 *
 * \tparam Range Type of the range.
 * \param[in] range Range of Eigen::Triplet.
 * \return A vector of Eigen::Triplet.
 */
template <std::ranges::viewable_range Range>
[[nodiscard]] auto to_vector(Range&& range)
    -> vector<typename std::ranges::range_value_t<Range>> {
    using triplet_type = typename std::ranges::range_value_t<Range>;
    vector<triplet_type> result;
    result.append_range(std::forward<Range>(range));
    return result;
}

//! Namespace of internal implementation.
namespace impl {

/*!
 * \brief Class of range adaptor closure to convert a range of Eigen::Triplet to
 * a vector of Eigen::Triplet.
 */
class to_vector_closure
    : public num_collect::util::impl::range_adaptor_closure<to_vector_closure> {
public:
    /*!
     * \brief Convert a range of Eigen::Triplet to a vector of Eigen::Triplet.
     *
     * \tparam Range Type of the range.
     * \param[in] range Range of Eigen::Triplet.
     * \return A vector of Eigen::Triplet.
     */
    template <std::ranges::viewable_range Range>
    [[nodiscard]] auto operator()(Range&& range) const
        -> vector<typename std::ranges::range_value_t<Range>> {
        return to_vector(std::forward<Range>(range));
    }
};

}  // namespace impl

/*!
 * \brief Create a range adaptor closure to convert a range of Eigen::Triplet to
 * a vector of Eigen::Triplet.
 *
 * \return A range adaptor closure to convert a range of Eigen::Triplet to a
 * vector of Eigen::Triplet.
 */
[[nodiscard]] inline auto to_vector() { return impl::to_vector_closure(); }

/*!
 * \brief Convert a range of Eigen::Triplet to a sparse matrix.
 *
 * \tparam SparseMatrix Type of the sparse matrix.
 * \tparam Range Type of the range.
 * \param[in] range Range of Eigen::Triplet.
 * \param[in] rows Number of rows of the sparse matrix.
 * \param[in] cols Number of columns of the sparse matrix.
 * \return A sparse matrix constructed from the range of Eigen::Triplet.
 */
template <base::concepts::sparse_matrix SparseMatrix,
    std::ranges::common_range Range>
[[nodiscard]] auto to_sparse_matrix(
    Range&& range, index_type rows, index_type cols) {
    SparseMatrix result(rows, cols);
    result.setFromTriplets(std::ranges::begin(range), std::ranges::end(range));
    return result;
}

namespace impl {

/*!
 * \brief Class of range adaptor closure to convert a range of Eigen::Triplet to
 * a sparse matrix.
 *
 * \tparam SparseMatrix Type of the sparse matrix.
 */
template <base::concepts::sparse_matrix SparseMatrix>
class to_sparse_matrix_closure
    : public num_collect::util::impl::range_adaptor_closure<
          to_sparse_matrix_closure<SparseMatrix>> {
public:
    /*!
     * \brief Constructor.
     *
     * \param [in] rows Number of rows of the sparse matrix.
     * \param [in] cols Number of columns of the sparse matrix.
     */
    to_sparse_matrix_closure(index_type rows, index_type cols)
        : rows_(rows), cols_(cols) {}

    /*!
     * \brief Convert a range of Eigen::Triplet to a sparse matrix.
     *
     * \tparam Range Type of the range.
     * \param[in] range Range of Eigen::Triplet.
     * \return A sparse matrix constructed from the range of Eigen::Triplet.
     */
    template <std::ranges::common_range Range>
    [[nodiscard]] auto operator()(Range&& range) const {
        return to_sparse_matrix<SparseMatrix>(
            std::forward<Range>(range), rows_, cols_);
    }

private:
    //! Number of rows of the sparse matrix.
    index_type rows_;

    //! Number of columns of the sparse matrix.
    index_type cols_;
};

}  // namespace impl

/*!
 * \brief Create a range adaptor closure to convert a range of Eigen::Triplet to
 * a sparse matrix.
 *
 * \tparam SparseMatrix Type of the sparse matrix.
 * \param[in] rows Number of rows of the sparse matrix.
 * \param[in] cols Number of columns of the sparse matrix.
 * \return A range adaptor closure to convert a range of Eigen::Triplet to a
 * sparse matrix.
 */
template <base::concepts::sparse_matrix SparseMatrix>
[[nodiscard]] inline auto to_sparse_matrix(index_type rows, index_type cols) {
    return impl::to_sparse_matrix_closure<SparseMatrix>(rows, cols);
}

/*!
 * \brief Create a range of Eigen::Triplet of diagonal matrix with a specified
 * value.
 *
 * \tparam Scalar Type of the scalar value.
 * \tparam StorageIndex Type of the storage index.
 * \param[in] size Size of the diagonal matrix.
 * \param[in] value Value to fill the diagonal.
 * \param[in] row_offset Row index offset for the diagonal entries.
 * \param[in] column_offset Column index offset for the diagonal entries.
 * \return A range of Eigen::Triplet representing the diagonal matrix.
 */
template <typename Scalar, typename StorageIndex = int>
[[nodiscard]] auto constant_diagonal_triplets(index_type size, Scalar value,
    index_type row_offset = 0, index_type column_offset = 0) {
    const StorageIndex size_storage_index = static_cast<StorageIndex>(size);
    const StorageIndex row_offset_storage_index =
        static_cast<StorageIndex>(row_offset);
    const StorageIndex column_offset_storage_index =
        static_cast<StorageIndex>(column_offset);
    return std::views::iota(static_cast<StorageIndex>(0), size_storage_index) |
        std::views::transform([value, row_offset_storage_index,
                                  column_offset_storage_index](auto i) {
            return Eigen::Triplet<Scalar, StorageIndex>(
                i + row_offset_storage_index, i + column_offset_storage_index,
                value);
        });
}

/*!
 * \brief Create a range of Eigen::Triplet of identity matrix.
 *
 * \tparam Scalar Type of the scalar value.
 * \tparam StorageIndex Type of the storage index.
 * \param[in] size Size of the identity matrix.
 * \param[in] row_offset Row index offset for the diagonal entries.
 * \param[in] column_offset Column index offset for the diagonal entries.
 * \return A range of Eigen::Triplet representing the identity matrix.
 */
template <typename Scalar, typename StorageIndex = int>
[[nodiscard]] auto identity_triplets(
    index_type size, index_type row_offset = 0, index_type column_offset = 0) {
    return constant_diagonal_triplets<Scalar, StorageIndex>(
        size, static_cast<Scalar>(1), row_offset, column_offset);
}

}  // namespace num_collect::util::eigen_triplets
