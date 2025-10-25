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
 * \brief Definition of floor function.
 */
#pragma once

#include <cmath>

#include "num_collect/functions/trunc.h"

namespace num_collect::functions {

namespace impl {

/*!
 * \brief Calculate the largest integer not greater than x, \f$ \lfloor x
 * \rfloor \f$ at compile time.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Largest integer not greater than x.
 */
template <typename T>
constexpr auto floor_at_compile_time(T x) -> T {
    if (x >= static_cast<T>(0)) {
        return trunc(x);
    }

    T trunc_x = trunc(x);
    if (x == trunc_x) {
        return x;
    }
    return trunc_x - static_cast<T>(1);
}

}  // namespace impl

/*!
 * \brief Calculate the largest integer not greater than x, \f$ \lfloor x
 * \rfloor \f$.
 *
 * This function calculates similar values as
 * [floor](https://en.cppreference.com/w/cpp/numeric/math/floor) function in C++
 * standard library in constexpr.
 *
 * \note This function assumes that inputs are floating-point numbers.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Largest integer not greater than x.
 */
template <typename T>
constexpr auto floor(T x) -> T {
    if consteval {
        return impl::floor_at_compile_time(x);
    } else {
        return std::floor(x);
    }
}

}  // namespace num_collect::functions
