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
 * \brief Definition of pow_pos_int function.
 */
#pragma once

#include <type_traits>

#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/two.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep

namespace num_collect::constants::impl {

/*!
 * \brief Calculate the value of base raised to the power exp.
 *
 * \warning Exponent is assumed to be a positive integer.
 *
 * \tparam T Base type.
 * \tparam I Integer type for exponent.
 * \param[in] base Base.
 * \param[in] exp Exponent.
 * \return Value of base raised to the power exp.
 */
template <typename T, typename I>
constexpr auto pow_pos_int(T base, I exp) -> T {
    if (exp == zero<I>) {
        return one<T>;
    }
    using unsigned_integer_type = std::make_unsigned_t<I>;
    auto remaining_exp = static_cast<unsigned_integer_type>(exp);
    T result = one<T>;
    while (remaining_exp > zero<unsigned_integer_type>) {
        if ((remaining_exp & one<unsigned_integer_type>) !=
            zero<unsigned_integer_type>) {
            result *= base;
        }
        base *= base;
        remaining_exp >>= 1U;
    }
    return result;
}

}  // namespace num_collect::constants::impl
