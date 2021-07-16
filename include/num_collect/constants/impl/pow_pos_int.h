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

#include "num_collect/constants/one.h"
#include "num_collect/constants/two.h"
#include "num_collect/constants/zero.h"

namespace num_collect::constants::impl {

/*!
 * \brief Calculate the value of base raised to the power exp.
 *
 * \warning Exponent is assumed to be a positive number.
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
    if (exp == one<I>) {
        return base;
    }
    if (exp == two<I>) {
        return base * base;
    }
    I exp1 = exp / two<I>;
    I exp2 = exp - exp1;
    return pow_pos_int(base, exp1) * pow_pos_int(base, exp2);
}

}  // namespace num_collect::constants::impl
