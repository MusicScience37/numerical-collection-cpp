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

#include <Eigen/SparseCore>

#include "num_collect/base/index_type.h"

//! Namespace of utility functions for Eigen::Triplet.
namespace num_collect::util::eigen_triplets {

/*!
 * \brief Create a range adaptor that shifts row indices of Eigen::Triplet.
 *
 * \param[in] row_offset The offset to add to the row indices.
 * \return A range adaptor that shifts row indices.
 */
auto shift_rows(index_type row_offset) {
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
 */
auto shift_columns(index_type column_offset) {
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
 */
auto shift_rows_and_columns(index_type row_offset, index_type column_offset) {
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
auto filter_rows(index_type row_start, index_type row_end) {
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
auto filter_columns(index_type column_start, index_type column_end) {
    return std::views::filter([column_start, column_end](auto&& triplet) {
        return triplet.col() >= column_start && triplet.col() < column_end;
    });
}

}  // namespace num_collect::util::eigen_triplets
