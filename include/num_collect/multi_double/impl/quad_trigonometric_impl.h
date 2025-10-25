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

#include <algorithm>

#include "num_collect/multi_double/impl/quad_integer_convertion_impl.h"
#include "num_collect/multi_double/impl/quad_internal_constants.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_constants.h"
#include "num_collect/util/assert.h"

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

/*!
 * \brief Calculate sin function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto sin_impl(quad x) noexcept -> quad {
    const quad two_pi_ratio = x * two_pi_inv_quad;
    const quad two_pi_count = floor_impl(two_pi_ratio + 0.5);
    const quad two_pi_reduced_x = x - two_pi_count * two_pi_quad;
    // Here two_pi_reduced_x is in [-pi, pi].

    const quad pi_over_4_ratio = two_pi_reduced_x * pi_over_4_inv_quad;
    int pi_over_4_count = static_cast<int>(floor_impl(pi_over_4_ratio).high());
    pi_over_4_count = std::min(std::max(pi_over_4_count, -4), 3);

    switch (pi_over_4_count) {
    case -4:
        return -sin_maclaurin(two_pi_reduced_x + pi_quad);
    case -3:
    case -2:
        return -cos_maclaurin(two_pi_reduced_x + pi_over_2_quad);
    case -1:
    case 0:
        return sin_maclaurin(two_pi_reduced_x);
    case 1:
    case 2:
        return cos_maclaurin(two_pi_reduced_x - pi_over_2_quad);
    case 3:
        return -sin_maclaurin(two_pi_reduced_x - pi_quad);
    default:
        // Unreachable.
        NUM_COLLECT_ASSERT(false);
    }
}

/*!
 * \brief Calculate cos function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto cos_impl(quad x) noexcept -> quad {
    const quad two_pi_ratio = x * two_pi_inv_quad;
    const quad two_pi_count = floor_impl(two_pi_ratio);
    const quad two_pi_reduced_x = x - two_pi_count * two_pi_quad;

    const quad pi_over_4_ratio = two_pi_reduced_x * pi_over_4_inv_quad;
    int pi_over_4_count = static_cast<int>(floor_impl(pi_over_4_ratio).high());
    pi_over_4_count = std::min(std::max(pi_over_4_count, 0), 7);

    switch (pi_over_4_count) {
    case 0:
        return cos_maclaurin(two_pi_reduced_x);
    case 1:
    case 2:
        return -sin_maclaurin(two_pi_reduced_x - pi_over_2_quad);
    case 3:
    case 4:
        return -cos_maclaurin(two_pi_reduced_x - pi_quad);
    case 5:
    case 6:
        return sin_maclaurin(two_pi_reduced_x - three_pi_over_2_quad);
    case 7:
        return cos_maclaurin(two_pi_reduced_x - two_pi_quad);
    default:
        // Unreachable.
        NUM_COLLECT_ASSERT(false);
    }
}

/*!
 * \brief Calculate tan function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto tan_impl(quad x) noexcept -> quad {
    return sin_impl(x) / cos_impl(x);
}

}  // namespace num_collect::multi_double::impl
