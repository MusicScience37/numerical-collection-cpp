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

}  // namespace num_collect::util::eigen_triplets
