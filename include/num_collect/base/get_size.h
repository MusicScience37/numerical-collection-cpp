/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of get_size function.
 */
#pragma once

#include <complex>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"

namespace num_collect {
inline namespace base {

/*!
 * \brief Get the number of elements.
 *
 * \tparam Matrix Type of matrix.
 * \param[in] matrix Matrix.
 * \return Number of elements (number of rows times number of columns).
 */
template <concepts::dense_matrix Matrix>
auto get_size(const Matrix& matrix) -> index_type {
    return matrix.size();
}

/*!
 * \brief Get the number of elements. (Overload for scalars.)
 *
 * \tparam T Type of the number.
 * \param[in] val Value.
 * \return Size.
 *
 * \note For this case, this function always returns 1.
 */
template <concepts::real_scalar T>
auto get_size(const T& val) -> index_type {
    (void)val;
    return 1;
}

/*!
 * \brief Get the number of elements. (Overload for scalars.)
 *
 * \tparam T Type of the number.
 * \param[in] val Value.
 * \return Size.
 *
 * \note For this case, this function always returns 1.
 */
template <concepts::real_scalar T>
auto get_size(const std::complex<T>& val) -> index_type {
    (void)val;
    return 1;
}

}  // namespace base
}  // namespace num_collect
