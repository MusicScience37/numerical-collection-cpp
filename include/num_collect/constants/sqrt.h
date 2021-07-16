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
 * \brief Definition of sqrt function.
 */
#pragma once

#include <limits>
#include <type_traits>

#include "num_collect/constants/half.h"
#include "num_collect/constants/one.h"
#include "num_collect/constants/zero.h"

namespace num_collect::constants {

/*!
 * \brief Calculate square root.
 *
 * \tparam T Value type.
 * \param[in] x Value to calculate square root of.
 * \return Square root.
 */
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr auto sqrt(T x) -> T {
    if (x < zero<T>) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    if ((x > std::numeric_limits<T>::max()) ||
        (x < std::numeric_limits<T>::min())) {
        return x;
    }

    constexpr int max_loops = 1000;  // safe guard
    T value = half<T> * (x + one<T>);
    for (int i = 0; i < max_loops; ++i) {
        T next_value = half<T> * (value + x / value);
        if (value == next_value) {
            break;
        }
        value = next_value;
    }

    return value;
}

}  // namespace num_collect::constants
