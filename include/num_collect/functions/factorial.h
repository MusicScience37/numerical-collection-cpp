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
 * \brief Definition of factorial function.
 */
#pragma once

#include <concepts>
#include <limits>

namespace num_collect::functions {

/*!
 * \brief Calculate factorial of n, \f$ n! \f$.
 *
 * \tparam Result Type of the result.
 * \tparam Integer Type of the integer.
 * \param[in] n Integer.
 * \return Factorial of n.
 *
 * If negative integers are given,
 * this function returns NaN for floating-point Result,
 * and returns 0 for integral Result.
 *
 * If too large integers are given,
 * this function will overflow.
 */
template <typename Result, std::integral Integer>
    requires(std::floating_point<Result> || std::integral<Result>)
constexpr auto factorial(const Integer n) noexcept -> Result {
    if constexpr (std::signed_integral<Integer>) {
        if constexpr (std::floating_point<Result>) {
            if (n < 0) {
                return std::numeric_limits<Result>::quiet_NaN();
            }
        } else {
            if (n < 0) {
                return 0;
            }
        }
    }
    Result result = static_cast<Result>(1);
    for (Integer i = 2; i <= n; ++i) {
        result *= static_cast<Result>(i);
    }
    return result;
}

}  // namespace num_collect::functions
