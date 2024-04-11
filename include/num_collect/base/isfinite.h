/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of isfinite function.
 */
#pragma once

#include <cmath>
#include <complex>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep

namespace num_collect {
inline namespace base {

/*!
 * \brief Check whether a number is finite.
 *
 * \tparam T Floating-point number type.
 * \param[in] val Number.
 * \retval true The input number is finite.
 * \retval false The input number is infinite or NAN.
 */
template <concepts::real_scalar T>
auto isfinite(const T& val) -> bool {
    using std::isfinite;
    return isfinite(val);
}

/*!
 * \brief Check whether a number is finite.
 *
 * \tparam T Floating-point number type.
 * \param[in] val Number.
 * \retval true The input number is finite.
 * \retval false The input number is infinite or NAN.
 */
template <concepts::real_scalar T>
auto isfinite(const std::complex<T>& val) -> bool {
    using std::isfinite;
    return isfinite(val.real()) && isfinite(val.imag());
}

}  // namespace base
}  // namespace num_collect
