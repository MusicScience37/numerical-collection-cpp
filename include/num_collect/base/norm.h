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
 * \brief Definition of norm class.
 */
#pragma once

#include <cmath>
#include <complex>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/integral.h"
#include "num_collect/base/concepts/real_scalar.h"

namespace num_collect {
inline namespace base {

/*!
 * \brief Calculate L2 norm of a matrix.
 *
 * \tparam Matrix Matrix type.
 * \param[in] matrix Matrix.
 * \return Norm.
 */
template <concepts::dense_matrix Matrix>
auto norm(const Matrix& matrix) {
    return matrix.norm();
}

/*!
 * \brief Calculate the absolute value of a number.
 *
 * \tparam T Floating-point number type.
 * \param[in] val Number.
 * \return Absolute value.
 */
template <concepts::real_scalar T>
auto norm(const T& val) -> T {
    using std::abs;
    return abs(val);
}

/*!
 * \brief Calculate the absolute value of a complex number.
 *
 * \tparam T Floating-point number type.
 * \param[in] val Number.
 * \return Absolute value.
 */
template <concepts::real_scalar T>
auto norm(const std::complex<T>& val) -> T {
    return std::abs(val);
}

/*!
 * \brief Calculate the absolute value of a number.
 *
 * \tparam T Integer type.
 * \param[in] val Number.
 * \return Absolute value.
 */
template <concepts::integral T>
auto norm(const T& val) -> T {
    using std::abs;
    return abs(val);
}

}  // namespace base
}  // namespace num_collect
