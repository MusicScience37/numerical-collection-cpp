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

#include <concepts>
#include <type_traits>

#include "num_collect/functions/exp.h"
#include "num_collect/functions/impl/pow_pos_int.h"
#include "num_collect/functions/log.h"
#include "num_collect/functions/trunc.h"

namespace num_collect::functions {

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
template <typename B, std::integral E>
constexpr auto pow(B base, E exp) -> B {
    if constexpr (std::is_signed_v<E>) {
        if (exp < static_cast<E>(0)) {
            return pow(static_cast<B>(1) / base, -exp);
        }
    }
    return impl::pow_pos_int(base, exp);
}

namespace impl {

/*!
 * \brief Calculate power \f$ {base}^{exp} \f$ at compile time.
 *
 * \tparam T Number type.
 * \param[in] base Base.
 * \param[in] exp Exponent.
 * \return Power.
 */
template <std::floating_point T>
constexpr auto pow_at_compile_time(T base, T exp) -> T {
    int int_part = static_cast<int>(trunc(exp));
    T rem_part = exp - static_cast<T>(int_part);
    return pow(base, int_part) * exp_at_compile_time(rem_part * log(base));
}

}  // namespace impl

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
template <std::floating_point T>
constexpr auto pow(T base, T exp) -> T {
    if consteval {
        return impl::pow_at_compile_time(base, exp);
    } else {
        return std::pow(base, exp);
    }
}

}  // namespace num_collect::functions
