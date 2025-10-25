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

#include <cmath>
#include <limits>
#include <type_traits>

#include "num_collect/functions/exp.h"
#include "num_collect/functions/impl/log1m_maclaurin.h"

namespace num_collect::functions {

namespace impl {

/*!
 * \brief Calculate logarithm \f$ \log(x) \f$ at compile time.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Logarithm
 */
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr auto log_at_compile_time(T x) -> T {
    if (x < static_cast<T>(0)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    if (x == static_cast<T>(0)) {
        return -std::numeric_limits<T>::infinity();
    }

    if (x > static_cast<T>(1)) {
        return -log_at_compile_time(static_cast<T>(1) / x);
    }

    T value = impl::log1m_maclaurin(static_cast<T>(1) - x);
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

}  // namespace impl

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
    if consteval {
        return impl::log_at_compile_time(x);
    } else {
        return std::log(x);
    }
}

}  // namespace num_collect::functions
