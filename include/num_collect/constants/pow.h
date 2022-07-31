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
 * \brief Definition of pow function.
 */
#pragma once

#include <type_traits>

#include "num_collect/constants/exp.h"
#include "num_collect/constants/impl/pow_pos_int.h"
#include "num_collect/constants/log.h"
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep

namespace num_collect::constants {

/*!
 * \brief Calculate power \f$ {base}^{exp} \f$.
 *
 * This function calculates similar values as
 * [pow](https://en.cppreference.com/w/cpp/numeric/math/pow) function in C++
 * standard library in constexpr.
 *
 * \tparam B Base type.
 * \tparam E Exponent type.
 * \param[in] base Base.
 * \param[in] exp Exponent.
 * \return Power.
 */
template <typename B, typename E,
    std::enable_if_t<std::is_integral_v<E>, void*> = nullptr>
constexpr auto pow(B base, E exp) -> B {
    if constexpr (std::is_signed_v<E>) {
        if (exp < zero<E>) {
            return one<B> / pow(base, -exp);
        }
    }
    return impl::pow_pos_int(base, exp);
}

/*!
 * \brief Calculate power \f$ {base}^{exp} \f$.
 *
 * This function calculates similar values as
 * [pow](https://en.cppreference.com/w/cpp/numeric/math/pow) function in C++
 * standard library in constexpr.
 *
 * \tparam T Number type.
 * \param[in] base Base.
 * \param[in] exp Exponent.
 * \return Power.
 */
template <typename T,
    std::enable_if_t<std::is_floating_point_v<T>, void*> = nullptr>
constexpr auto pow(T base, T exp) -> T {
    return ::num_collect::constants::exp(exp * log(base));
}

}  // namespace num_collect::constants
