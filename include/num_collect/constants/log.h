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
 * \brief Definition of log function.
 */
#pragma once

#include <limits>
#include <type_traits>

#include "num_collect/constants/exp.h"
#include "num_collect/constants/impl/log1m_maclaurin.h"
#include "num_collect/constants/one.h"

namespace num_collect::constants {

/*!
 * \brief Calculate logarithm \f$ \log(x) \f$.
 *
 * This function calculates similar values as
 * [log](https://en.cppreference.com/w/cpp/numeric/math/log) function in C++
 * standard library in constexpr.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Logarithm
 */
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr auto log(T x) -> T {
    if (x < zero<T>) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    if (x == zero<T>) {
        return -std::numeric_limits<T>::infinity();
    }

    if (x > one<T>) {
        return -log(one<T> / x);
    }

    T value = impl::log1m_maclaurin(one<T> - x);
    constexpr int max_loops = 1000;
    for (int i = 0; i < max_loops; ++i) {
        T exp_val = exp(value);
        T next_value = value - (exp_val - x) / exp_val;
        if (value == next_value) {
            break;
        }
        value = next_value;
    }

    return value;
}

}  // namespace num_collect::constants
