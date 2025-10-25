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
 * \brief Definition of exp function.
 */
#pragma once

#include <cmath>
#include <type_traits>

#include "num_collect/constants/exp.h"
#include "num_collect/constants/impl/expm1_maclaurin.h"
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep

namespace num_collect::constants {

namespace impl {

/*!
 * \brief Calculate exponential function minus one \f$ e^x - 1 \f$ at compile
 * time.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Exponential function minus one.
 */
template <typename T>
constexpr auto expm1_at_compile_time(T x) -> T {
    if (x < -one<T> || one<T> < x) {
        return exp(x) - one<T>;
    }

    if (x >= zero<T>) {
        return expm1_maclaurin(x);
    }

    T expm1_neg = expm1_maclaurin(-x);
    return -expm1_neg / (expm1_neg + one<T>);
}

}  // namespace impl

/*!
 * \brief Calculate exponential function minus one \f$ e^x - 1 \f$.
 *
 * This function calculates similar values as
 * [expm1](https://en.cppreference.com/w/cpp/numeric/math/expm1) function in C++
 * standard library in constexpr.
 *
 * This function has following optimization for small values:
 *
 * - for \f$ 0 \le x \le 1 \f$, Maclaurin series for \f$ e^x - 1 \f$ is used.
 * - for \f$ -1 \le x < 0 \f$, \f$ e^x - 1 = - (e^{-x} - 1) / e^{-x} \f$ is
 *   calculated.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Exponential function minus one.
 */
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr auto expm1(T x) -> T {
    if consteval {
        return impl::expm1_at_compile_time(x);
    } else {
        return std::expm1(x);
    }
}

}  // namespace num_collect::constants
