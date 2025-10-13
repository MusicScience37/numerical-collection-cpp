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
 * \brief Definition of internal functions to calculate trigonometric functions
 * for quad class.
 */
#pragma once

#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

//! Number of terms in Maclaurin series for sin and cos functions.
constexpr int num_sin_cos_maclaurin_terms = 13;

/*!
 * \brief Calculate sin function using Maclaurin series.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto sin_maclaurin(quad x) noexcept -> quad {
    const quad negative_square_x = -x * x;
    quad term = x;
    quad result = 0.0;
    for (int i = 1; i <= num_sin_cos_maclaurin_terms; ++i) {
        term *= negative_square_x;
        term /= (2 * i) * (2 * i + 1);
        result += term;
    }
    result += x;
    return result;
}

/*!
 * \brief Calculate cos function using Maclaurin series.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto cos_maclaurin(quad x) noexcept -> quad {
    const quad negative_square_x = -x * x;
    quad term = 1.0;
    quad result = 0.0;
    for (int i = 1; i <= num_sin_cos_maclaurin_terms; ++i) {
        term *= negative_square_x;
        term /= (2 * i - 1) * (2 * i);
        result += term;
    }
    result += 1.0;
    return result;
}

}  // namespace num_collect::multi_double::impl
