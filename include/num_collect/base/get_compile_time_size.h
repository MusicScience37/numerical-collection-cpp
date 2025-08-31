/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of get_compile_time_size function.
 */
#pragma once

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"

namespace num_collect {
inline namespace base {

/*!
 * \brief Get the number of elements at compile time.
 *
 * \tparam T Type of the matrix.
 * \return Number of elements (number of rows times number of columns).
 *
 * \note This function is defined only when the matrix is not dynamic.
 */
template <concepts::dense_matrix T>
    requires(T::RowsAtCompileTime >= 0 && T::ColsAtCompileTime >= 0)
constexpr auto get_compile_time_size() -> index_type {
    return T::RowsAtCompileTime * T::ColsAtCompileTime;
}

/*!
 * \brief Get the number of elements at compile time. (Overload for scalars.)
 *
 * \tparam T Type of the matrix.
 * \return Always 1 for this overload.
 */
template <concepts::real_scalar T>
constexpr auto get_compile_time_size() -> index_type {
    return 1;
}

}  // namespace base
}  // namespace num_collect
