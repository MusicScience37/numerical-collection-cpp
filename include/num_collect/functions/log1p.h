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
 * \brief Definition of log1p function.
 */
#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

#include "num_collect/functions/expm1.h"
#include "num_collect/functions/impl/log1m_maclaurin.h"
#include "num_collect/functions/log.h"

namespace num_collect::functions {

namespace impl {

/*!
 * \brief Calculate natural logarithm of 1 + x, \f$ \log(1 + x) \f$ at compile
 * time.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Logarithm of 1 + x.
 */
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr auto log1p_at_compile_time(T x) -> T {
    if (x < -static_cast<T>(1)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    if (x == -static_cast<T>(1)) {
        return -std::numeric_limits<T>::infinity();
    }

    if (x > static_cast<T>(1)) {
        return log(x + static_cast<T>(1));
    }
    if (x > static_cast<T>(0)) {
        return -log1p_at_compile_time(-x / (x + static_cast<T>(1)));
    }

    T value = x;
    if (x > static_cast<T>(0)) {
        value = -log1p_at_compile_time(-x / (x + static_cast<T>(1)));
    } else {
        value = impl::log1m_maclaurin(-x);
    }
    constexpr int max_loops = 1000;
    for (int i = 0; i < max_loops; ++i) {
        T expm1_val = expm1(value);
        T next_value =
            value - (expm1_val - x) / (expm1_val + static_cast<T>(1));
        if (value == next_value) {
            break;
        }
        value = next_value;
    }

    return value;
}

}  // namespace impl

/*!
 * \brief Calculate natural logarithm of 1 + x, \f$ \log(1 + x) \f$.
 *
 * This function calculates similar values as
 * [log1p](https://en.cppreference.com/w/cpp/numeric/math/log1p) function in C++
 * standard library in constexpr.
 *
 * This function can calculate natural logarithm of numbers near to 1 more
 * accurately.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Logarithm of 1 + x.
 */
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr auto log1p(T x) -> T {
    if consteval {
        return impl::log1p_at_compile_time(x);
    } else {
        return std::log1p(x);
    }
}

}  // namespace num_collect::functions
