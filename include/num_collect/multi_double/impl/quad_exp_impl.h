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
 * \brief Definition of functions of internal implementations of exp function
 * for quad class.
 */
#pragma once

#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate exponential minus one \f$ e^x - 1 \f$ using Maclaurin
 * series.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function can handle values satisfying
 * \f$ |x| \leq 1.36 \times 10^{-3} \f$
 * with relative error up to \f$ 2^{-102} \f$.
 */
inline auto expm1_maclaurin_series(quad x) noexcept -> quad {
    constexpr int num_terms = 9;
    quad term = x;
    quad result = term;
    for (int i = 2; i <= num_terms; ++i) {
        term *= x;
        term /= i;
        result += term;
    }
    return result;
}

/*!
 * \brief Calculate exponential \f$ e^x \f$ using Maclaurin series.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function can handle values satisfying
 * \f$ |x| \leq 1.36 \times 10^{-3} \f$
 * with relative error up to \f$ 2^{-102} \f$.
 */
inline auto exp_maclaurin_series(quad x) noexcept -> quad {
    return expm1_maclaurin_series(x) + quad(1.0);
}

}  // namespace num_collect::multi_double::impl
