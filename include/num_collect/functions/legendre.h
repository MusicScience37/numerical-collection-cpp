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
 * \brief Definition of legendre function.
 */
#pragma once

#include <concepts>
#include <limits>
#include <utility>

#include "num_collect/base/concepts/real_scalar.h"

namespace num_collect::functions {

/*!
 * \brief Calculate Legendre function.
 *
 * \tparam F Type of x (floating-point number).
 * \tparam I Type of n (integer).
 * \param[in] x Variable.
 * \param[in] n Degree of the Legendre function.
 * \return Value of n-thLegendre function of x.
 */
template <base::concepts::real_scalar F, std::integral I>
constexpr auto legendre(F x, I n) -> F {
    if constexpr (std::is_signed_v<I>) {
        if (n < static_cast<I>(0)) {
            return std::numeric_limits<F>::quiet_NaN();
        }
    }
    if (n == static_cast<I>(0)) {
        return static_cast<F>(1);
    }
    if (n == static_cast<I>(1)) {
        return x;
    }
    F y_p = static_cast<F>(0);
    F y = x;
    F y_m = static_cast<F>(1);
    for (I i = static_cast<I>(1); i < n; ++i) {
        y_p =
            (static_cast<F>(static_cast<I>(2) * i + static_cast<I>(1)) * x * y -
                static_cast<F>(i) * y_m) /
            static_cast<F>(i + static_cast<I>(1));
        y_m = y;
        y = y_p;
    }
    return y;
}

/*!
 * \brief Calculate Legendre function and its differential coefficient.
 *
 * \tparam F Type of x (floating-point number).
 * \tparam I Type of n (integer).
 * \param[in] x Variable.
 * \param[in] n Degree of the Legendre function.
 * \return n-thLegendre function and its differential coefficient of x.
 */
template <base::concepts::real_scalar F, std::integral I>
constexpr auto legendre_with_diff(F x, I n) -> std::pair<F, F> {
    if constexpr (std::is_signed_v<I>) {
        if (n < static_cast<I>(0)) {
            return {std::numeric_limits<F>::quiet_NaN(),
                std::numeric_limits<F>::quiet_NaN()};
        }
    }
    if (n == static_cast<I>(0)) {
        return {static_cast<F>(1), static_cast<F>(0)};
    }
    if (n == static_cast<I>(1)) {
        return {x, static_cast<F>(1)};
    }

    if (x == static_cast<F>(1)) {
        return {static_cast<F>(1),
            static_cast<F>(0.5) * static_cast<F>(n) *
                static_cast<F>(n + static_cast<I>(1))};
    }
    if (x == -static_cast<F>(1)) {
        if (n % static_cast<I>(2) == static_cast<I>(0)) {
            return {static_cast<F>(1),
                -static_cast<F>(0.5) * static_cast<F>(n) *
                    static_cast<F>(n + static_cast<I>(1))};
        }
        return {-static_cast<F>(1),
            static_cast<F>(0.5) * static_cast<F>(n) *
                static_cast<F>(n + static_cast<I>(1))};
    }

    F y_p = static_cast<F>(0);
    F y = x;
    F y_m = static_cast<F>(1);
    for (I i = static_cast<I>(1); i < n; ++i) {
        y_p =
            (static_cast<F>(static_cast<I>(2) * i + static_cast<I>(1)) * x * y -
                static_cast<F>(i) * y_m) /
            static_cast<F>(i + static_cast<I>(1));
        y_m = y;
        y = y_p;
    }

    F dif = static_cast<F>(n) * (y_m - x * y) / (static_cast<F>(1) - x * x);

    return {y, dif};
}

}  // namespace num_collect::functions
