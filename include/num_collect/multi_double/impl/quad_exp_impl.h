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

#include <cmath>
#include <limits>

#include "num_collect/constants/floor.h"
#include "num_collect/multi_double/impl/quad_internal_constants.h"
#include "num_collect/multi_double/impl/quad_ldexp_impl.h"
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
constexpr auto expm1_maclaurin_series(quad x) noexcept -> quad {
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
constexpr auto exp_maclaurin_series(quad x) noexcept -> quad {
    return expm1_maclaurin_series(x) + 1.0;
}

/*!
 * \brief Round a double value to the nearest integer value.
 *
 * \param[in] value Input value.
 * \return Rounded value.
 */
constexpr auto round_double_to_int(double value) noexcept -> int {
    if consteval {
        // std::lround is not constexpr.
        // NOLINTNEXTLINE(*-magic-numbers)
        return static_cast<int>(constants::floor(value + 0.5));
    } else {
        return static_cast<int>(std::lround(value));
    }
}

/*!
 * \brief Calculate exponential \f$ e^x \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
constexpr auto exp_impl(quad x) noexcept -> quad {
    const quad log2_rate = x * log2_inv_quad;

    constexpr double max_exponent =
        static_cast<double>(std::numeric_limits<double>::max_exponent);
    constexpr double min_exponent =
        static_cast<double>(std::numeric_limits<double>::min_exponent);
    if (log2_rate > max_exponent) {
        return quad(std::numeric_limits<double>::infinity());
    }
    if (log2_rate < min_exponent) {
        return quad(0.0);
    }

    const int two_exponent = round_double_to_int(log2_rate.high());
    const quad remainder = x - log2_quad * two_exponent;
    // Here |remainder| <= 0.5 * log(2) = 0.3465...

    constexpr unsigned int num_last_multiplication = 8;
    const quad reduced_remainder =
        ldexp_impl(remainder, -static_cast<int>(num_last_multiplication));
    // Here |reduced_remainder| <= 0.3465... / 256 < exp_maclaurin_limit_quad
    const quad reduced_exp = exp_maclaurin_series(reduced_remainder);

    quad remainder_exp = reduced_exp;
    // Unroll the loop for performance.
    remainder_exp *= remainder_exp;
    remainder_exp *= remainder_exp;
    remainder_exp *= remainder_exp;
    remainder_exp *= remainder_exp;
    remainder_exp *= remainder_exp;
    remainder_exp *= remainder_exp;
    remainder_exp *= remainder_exp;
    remainder_exp *= remainder_exp;

    return ldexp_impl(remainder_exp, two_exponent);
}

/*!
 * \brief Calculate exponential minus one \f$ e^x - 1 \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
constexpr auto expm1_impl(quad x) noexcept -> quad {
    if (-exp_maclaurin_limit_quad.high() <= x.high() &&
        x.high() <= exp_maclaurin_limit_quad.high()) {
        return expm1_maclaurin_series(x);
    }
    if (-half_log2_quad.high() <= x.high() &&
        x.high() <= half_log2_quad.high()) {
        constexpr unsigned int num_last_multiplication = 8;
        const quad reduced_x =
            ldexp_impl(x, -static_cast<int>(num_last_multiplication));
        // Here |reduced_x| <= 0.3465... / 256 < exp_maclaurin_limit_quad
        const quad reduced_expm1 = expm1_maclaurin_series(reduced_x);
        quad result = reduced_expm1;
        // Unroll the loop for performance.
        constexpr double two = 2.0;
        result = (result + two) * result;
        result = (result + two) * result;
        result = (result + two) * result;
        result = (result + two) * result;
        result = (result + two) * result;
        result = (result + two) * result;
        result = (result + two) * result;
        result = (result + two) * result;
        return result;
    }
    return exp_impl(x) - 1.0;
}

}  // namespace num_collect::multi_double::impl
