/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of sparse_matrix concept.
 */
#pragma once

#include <type_traits>

#include <Eigen/SparseCore>

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Check whether a type is a sparse matrix.
 *
 * \tparam T Type.
 */
template <typename T>
struct is_sparse_matrix : public std::false_type {};

/*!
 * \brief Check whether a type is a sparse matrix.
 *
 * \tparam Scalar Type of the scalar.
 * \tparam Options Options.
 * \tparam StorageIndex Type of storage indices.
 */
template <typename Scalar, int Options, typename StorageIndex>
struct is_sparse_matrix<Eigen::SparseMatrix<Scalar, Options, StorageIndex>>
    : public std::true_type {};

/*!
 * \brief Check whether a type is a sparse matrix.
 *
 * \tparam T Type.
 */
template <typename T>
constexpr bool is_sparse_matrix_v = is_sparse_matrix<T>::value;

/*!
 * \brief Concept of sparse matrices.
 *
 * \tparam T Type.
 */
template <typename T>
concept sparse_matrix = is_sparse_matrix_v<T>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
