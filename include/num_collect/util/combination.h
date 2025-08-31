/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of combination function.
 */
#pragma once

#include <concepts>
#include <type_traits>

#include "num_collect/base/exception.h"

namespace num_collect::util {

/*!
 * \brief Computes the combination \f$ {}_n C_k \f$.
 *
 * \tparam T Type of integers.
 * \param[in] n Total number of items.
 * \param[in] k Number of chosen items.
 * \return Combination \f$ {}_n C_k \f$. 0 when \f$ k > n \f$.
 * \throw num_collect::base::invalid_argument when a negative integer is given.
 *
 * \warning This function can overflow for large inputs.
 */
template <std::integral T>
constexpr auto combination(T n, T k) -> T {
    if (std::is_signed_v<T>) {
        if (n < 0 || k < 0) {
            throw invalid_argument(
                "Negative number was given to combination function.");
        }
    }
    if (k > n) {
        return 0;
    }
    if (k > n - k) {
        k = n - k;
    }
    T result = 1;
    // This loop won't overflow because such k is replaced with (n - k).
    for (T i = 1; i <= k; ++i) {
        result = result * (n - k + i) / i;
    }
    return result;
}

}  // namespace num_collect::util
