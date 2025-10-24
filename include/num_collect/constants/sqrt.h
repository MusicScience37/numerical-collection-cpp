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

#include <cmath>
#include <concepts>
#include <limits>

#include "num_collect/constants/half.h"  // IWYU pragma: keep
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep

namespace num_collect::constants {

namespace impl {

/*!
 * \brief Calculate square root \f$ \sqrt{x} \f$ at compile time.
 *
 * \tparam F Value type.
 * \param[in] x Value to calculate square root of.
 * \return Square root.
 */
template <std::floating_point F>
constexpr auto sqrt_at_compile_time(F x) -> F {
    if (x < zero<F>) {
        return std::numeric_limits<F>::quiet_NaN();
    }
    if ((x > std::numeric_limits<F>::max()) ||
        (x < std::numeric_limits<F>::min())) {
        return x;
    }

    constexpr int max_loops = 1000;  // safe guard
    F value = half<F> * (x + one<F>);
    for (int i = 0; i < max_loops; ++i) {
        F next_value = half<F> * (value + x / value);
        if (value == next_value) {
            break;
        }
        value = next_value;
    }

    return value;
}

}  // namespace impl

/*!
 * \brief Calculate square root \f$ \sqrt{x} \f$.
 *
 * This function calculates similar values as
 * [sqrt](https://en.cppreference.com/w/cpp/numeric/math/sqrt) function in C++
 * standard library in constexpr.
 *
 * \tparam F Value type.
 * \param[in] x Value to calculate square root of.
 * \return Square root.
 */
template <std::floating_point F>
constexpr auto sqrt(F x) -> F {
    if consteval {
        return impl::sqrt_at_compile_time(x);
    } else {
        return std::sqrt(x);
    }
}

/*!
 * \brief Calculate square root \f$ \sqrt{x} \f$.
 *
 * This function calculates similar values as
 * [sqrt](https://en.cppreference.com/w/cpp/numeric/math/sqrt) function in C++
 * standard library in constexpr.
 *
 * \tparam I Value type.
 * \param[in] x Value to calculate square root of.
 * \return Square root.
 */
template <std::integral I>
constexpr auto sqrt(I x) -> double {
    return sqrt(static_cast<double>(x));
}

}  // namespace num_collect::constants
