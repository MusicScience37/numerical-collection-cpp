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

#include <cmath>
#include <cstdint>
#include <limits>

namespace num_collect::functions {

namespace impl {

/*!
 * \brief Truncate the decimal part of a number x at compile time.
 *
 * \tparam T Number type.
 * \param[in] x Number to truncate.
 * \return Truncated number.
 */
template <typename T>
constexpr auto trunc_at_compile_time(T x) -> T {
    if (x < static_cast<T>(0)) {
        return -trunc_at_compile_time(-x);
    }
    if (x > static_cast<T>(std::numeric_limits<std::uintmax_t>::max())) {
        return x;
    }
    return static_cast<T>(static_cast<std::uintmax_t>(x));
}

}  // namespace impl

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
    if consteval {
        return impl::trunc_at_compile_time(x);
    } else {
        return std::trunc(x);
    }
}

}  // namespace num_collect::functions
