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
 * \brief Definition of gamma_half_plus function.
 */
#pragma once

#include <concepts>

#include "num_collect/constants/double_factorial.h"
#include "num_collect/constants/pi.h"
#include "num_collect/constants/pow.h"
#include "num_collect/constants/sqrt.h"
#include "num_collect/functions/gamma.h"

namespace num_collect::constants {

namespace impl {

/*!
 * \brief Calculate gamma function for integers plus half. \f$ \Gamma(n + 1/2)
 * \f$ at compile time.
 *
 * \tparam Result Type of the result, should be floating-point.
 * \tparam Integer Type of the integer.
 * \param[in] n Integer.
 * \return Value of \f$ \Gamma(n + 1/2) \f$.
 */
template <std::floating_point Result, std::integral Integer>
constexpr auto gamma_half_plus_at_compile_time(const Integer n) -> Result {
    if constexpr (std::signed_integral<Integer>) {
        if (n < 0) {
            // Negative integers.
            return pow(-2, -n) * sqrt(pi<Result>) /
                double_factorial<Result>(-n * 2 - 1);
        }
    }
    if (n == 0) {
        return sqrt(pi<Result>);
    }
    // Positive integers.
    return double_factorial<Result>(n * 2 - 1) * sqrt(pi<Result>) / pow(2, n);
}

}  // namespace impl

/*!
 * \brief Calculate gamma function for integers plus half. \f$ \Gamma(n + 1/2)
 * \f$.
 *
 * \tparam Result Type of the result, should be floating-point.
 * \tparam Integer Type of the integer.
 * \param[in] n Integer.
 * \return Value of \f$ \Gamma(n + 1/2) \f$.
 *
 * If integers with too large absolute value are given,
 * this function will overflow.
 */
template <std::floating_point Result, std::integral Integer>
constexpr auto gamma_half_plus(const Integer n) noexcept -> Result {
    if consteval {
        return impl::gamma_half_plus_at_compile_time<Result>(n);
    } else {
        return functions::gamma(
            static_cast<Result>(n) + static_cast<Result>(0.5));
    }
}

}  // namespace num_collect::constants
