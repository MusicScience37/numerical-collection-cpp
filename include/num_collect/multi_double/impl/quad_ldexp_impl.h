/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of ldexp_impl function for quad class.
 */
#pragma once

#include <cmath>

#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate \f$ x 2^{exp} \f$.
 *
 * \param[in] x Multiplied value.
 * \param[in] exp Exponent of 2.
 * \return Result.
 */
inline auto ldexp_impl(quad x, int exp) noexcept -> quad {
    const double exp2 = std::ldexp(1.0, exp);
    const double high = x.high() * exp2;
    const double low = x.low() * exp2;
    return quad(high, low);
}

}  // namespace num_collect::multi_double::impl
