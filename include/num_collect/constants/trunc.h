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
 * \brief Definition of trunc function.
 */
#pragma once

#include <cstdint>
#include <limits>

#include "num_collect/constants/zero.h"  // IWYU pragma: keep

namespace num_collect::constants {

/*!
 * \brief Truncate the decimal part of a number x.
 *
 * This function calculates similar values as
 * [trunc](https://en.cppreference.com/w/cpp/numeric/math/trunc) function in C++
 * standard library in constexpr.
 *
 * \note This function assumes that inputs are floating-point numbers.
 *
 * \tparam T Number type.
 * \param[in] x Number to truncate.
 * \return Truncated number.
 */
template <typename T>
constexpr auto trunc(T x) -> T {
    if (x < zero<T>) {
        return -trunc(-x);
    }
    if (x > static_cast<T>(std::numeric_limits<std::uintmax_t>::max())) {
        return x;
    }
    return static_cast<T>(static_cast<std::uintmax_t>(x));
}

}  // namespace num_collect::constants
