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

#include <type_traits>

#include "num_collect/constants/impl/exp_maclaurin.h"
#include "num_collect/constants/impl/pow_pos_int.h"
#include "num_collect/constants/napier.h"
#include "num_collect/constants/one.h"
#include "num_collect/constants/trunc.h"
#include "num_collect/constants/zero.h"

namespace num_collect::constants {

/*!
 * \brief Calculate exponential function \f$ e^x \f$.
 *
 * This function calculates similar values as
 * [exp](https://en.cppreference.com/w/cpp/numeric/math/exp) function in C++
 * standard library in constexpr.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return Exponential function value.
 */
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr auto exp(T x) -> T {
    if (x < zero<T>) {
        return one<T> / exp(-x);
    }
    if (x < std::numeric_limits<T>::min()) {
        return one<T>;
    }

    constexpr auto thresh_inf =
        static_cast<T>(std::numeric_limits<T>::max_exponent);
    if (x >= thresh_inf) {
        return std::numeric_limits<T>::infinity();
    }

    int int_part = static_cast<int>(trunc(x));
    T rem_part = x - static_cast<T>(int_part);
    T exp_int_part = impl::pow_pos_int(napier<T>, int_part);
    T exp_rem_part = impl::exp_maclaurin(rem_part);
    return exp_int_part * exp_rem_part;
}

}  // namespace num_collect::constants
