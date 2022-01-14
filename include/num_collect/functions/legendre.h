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
#include <type_traits>
#include <utility>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/constants/half.h"
#include "num_collect/constants/one.h"
#include "num_collect/constants/two.h"
#include "num_collect/constants/zero.h"

namespace num_collect::functions {

/*!
 * \brief Calculate Legendre function.
 *
 * \tparam F Type of x (floating-point number).
 * \tparam I Type of n (integer).
 * \param[in] x Variable.
 * \param[in] n Order of the Legendre function.
 * \return Value of n-thLegendre function of x.
 */
template <base::concepts::real_scalar F, std::integral I>
constexpr auto legendre(F x, I n) -> F {
    if constexpr (std::is_signed_v<I>) {
        if (n < constants::zero<I>) {
            return std::numeric_limits<F>::quiet_NaN();
        }
    }
    if (n == constants::zero<I>) {
        return constants::one<F>;
    }
    if (n == constants::one<I>) {
        return x;
    }
    F y_p = constants::zero<F>;
    F y = x;
    F y_m = constants::one<F>;
    for (I i = constants::one<I>; i < n; ++i) {
        y_p =
            (static_cast<F>(constants::two<I> * i + constants::one<I>) * x * y -
                static_cast<F>(i) * y_m) /
            static_cast<F>(i + constants::one<I>);
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
 * \param[in] n Order of the Legendre function.
 * \return n-thLegendre function and its differential coefficient of x.
 */
template <base::concepts::real_scalar F, std::integral I>
constexpr auto legendre_with_diff(F x, I n) -> std::pair<F, F> {
    if constexpr (std::is_signed_v<I>) {
        if (n < constants::zero<I>) {
            return {std::numeric_limits<F>::quiet_NaN(),
                std::numeric_limits<F>::quiet_NaN()};
        }
    }
    if (n == constants::zero<I>) {
        return {constants::one<F>, constants::zero<F>};
    }
    if (n == constants::one<I>) {
        return {x, constants::one<F>};
    }

    if (x == constants::one<F>) {
        return {constants::one<F>,
            constants::half<F> * static_cast<F>(n) *
                static_cast<F>(n + constants::one<I>)};
    }
    if (x == -constants::one<F>) {
        if (n % constants::two<I> == constants::zero<I>) {
            return {constants::one<F>,
                -constants::half<F> * static_cast<F>(n) *
                    static_cast<F>(n + constants::one<I>)};
        }
        return {-constants::one<F>,
            constants::half<F> * static_cast<F>(n) *
                static_cast<F>(n + constants::one<I>)};
    }

    F y_p = constants::zero<F>;
    F y = x;
    F y_m = constants::one<F>;
    for (I i = constants::one<I>; i < n; ++i) {
        y_p =
            (static_cast<F>(constants::two<I> * i + constants::one<I>) * x * y -
                static_cast<F>(i) * y_m) /
            static_cast<F>(i + constants::one<I>);
        y_m = y;
        y = y_p;
    }

    F dif = n * (y_m - x * y) / (constants::one<F> - x * x);

    return {y, dif};
}

}  // namespace num_collect::functions
