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

namespace num_collect::functions::impl {

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
    if (exp == static_cast<I>(0)) {
        return static_cast<T>(1);
    }
    using unsigned_integer_type = std::make_unsigned_t<I>;
    auto remaining_exp = static_cast<unsigned_integer_type>(exp);
    T result = static_cast<T>(1);
    {
        // First time.
        if ((remaining_exp & static_cast<unsigned_integer_type>(1)) !=
            static_cast<unsigned_integer_type>(0)) {
            result *= base;
        }
        remaining_exp >>= static_cast<unsigned_integer_type>(1);
    }
    while (remaining_exp > static_cast<unsigned_integer_type>(0)) {
        base *= base;
        if ((remaining_exp & static_cast<unsigned_integer_type>(1)) !=
            static_cast<unsigned_integer_type>(0)) {
            result *= base;
        }
        remaining_exp >>= static_cast<unsigned_integer_type>(1);
    }
    return result;
}

}  // namespace num_collect::functions::impl
