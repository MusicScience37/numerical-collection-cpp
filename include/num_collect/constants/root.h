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
 * \brief Definition of root function.
 */
#pragma once

#include <limits>
#include <type_traits>

#include "num_collect/constants/half.h"
#include "num_collect/constants/impl/pow_pos_int.h"
#include "num_collect/constants/one.h"
#include "num_collect/constants/zero.h"

namespace num_collect::constants {

/*!
 * \brief Calculate n-th root \f$ \sqrt[n]{x} \f$.
 *
 * \tparam F Value type.
 * \tparam I Integer type for n.
 * \param[in] x Value to calculate n-th root of.
 * \param[in] n Exponent.
 * \return n-th root of x.
 */
template <typename F, typename I,
    std::enable_if_t<std::is_floating_point_v<F> && std::is_integral_v<I>,
        void*> = nullptr>
constexpr auto root(F x, I n) -> F {
    if (n < 2) {
        return std::numeric_limits<F>::quiet_NaN();
    }
    if (x < zero<F>) {
        if ((n % 2) == 0) {
            return std::numeric_limits<F>::quiet_NaN();
        }
        return -root(-x, n);
    }
    if ((x > std::numeric_limits<F>::max()) ||
        (x < std::numeric_limits<F>::min())) {
        return x;
    }

    constexpr int max_loops = 1000;  // safe guard
    F value = one<F> + (x - one<F>) / static_cast<F>(n);
    for (int i = 0; i < max_loops; ++i) {
        F next_value = (static_cast<F>(n - one<I>) * value +
                           x / impl::pow_pos_int(value, n - one<I>)) /
            static_cast<F>(n);
        if (value == next_value) {
            break;
        }
        value = next_value;
    }

    return value;
}

/*!
 * \brief Calculate n-th root \f$ \sqrt[n]{x} \f$.
 *
 * \tparam IB Value type.
 * \tparam IE Integer type for n.
 * \param[in] x Value to calculate n-th root of.
 * \param[in] n Exponent.
 * \return n-th root of x.
 */
template <typename IB, typename IE,
    std::enable_if_t<std::is_integral_v<IB> && std::is_integral_v<IE>, void*> =
        nullptr>
constexpr auto root(IB x, IE n) -> double {
    return root(static_cast<double>(x), n);
}

}  // namespace num_collect::constants
