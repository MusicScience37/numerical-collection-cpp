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
 * \brief Definition of ceil function.
 */
#pragma once

#include "num_collect/constants/one.h"  // IWYU pragma: keep
#include "num_collect/constants/trunc.h"
#include "num_collect/constants/zero.h"  // IWYU pragma: keep

namespace num_collect::constants {

/*!
 * \brief Calculate the smallest integer not less than x, \f$ \lceil x \rceil
 * \f$.
 *
 * This function calculates similar values as
 * [ceil](https://en.cppreference.com/w/cpp/numeric/math/ceil) function in C++
 * standard library in constexpr.
 *
 * \note This function assumes that inputs are floating-point numbers.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Smallest integer not less than x.
 */
template <typename T>
constexpr auto ceil(T x) -> T {
    if (x <= zero<T>) {
        return trunc(x);
    }

    T trunc_x = trunc(x);
    if (x == trunc_x) {
        return x;
    }
    return trunc_x + one<T>;
}

}  // namespace num_collect::constants
