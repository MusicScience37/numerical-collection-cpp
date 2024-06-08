/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of greatest_common_divisor function.
 */
#pragma once

#include "num_collect/base/concepts/integral.h"
#include "num_collect/base/exception.h"

namespace num_collect::util {

/*!
 * \brief Calculate the greatest common divisor.
 *
 * \tparam T Type of integers.
 * \param[in] a An integer.
 * \param[in] b An integer.
 * \return Greatest common divisor of the integers.
 */
template <base::concepts::integral T>
[[nodiscard]] constexpr auto greatest_common_divisor(T a, T b) -> T {
    if (a <= static_cast<T>(0) || b <= static_cast<T>(0)) {
        throw invalid_argument(
            "greatest_common_divisor requires positive integers.");
    }
    while (true) {
        const T x = a % b;
        if (x == static_cast<T>(0)) {
            return b;
        }
        a = b;
        b = x;
    }
}

}  // namespace num_collect::util
