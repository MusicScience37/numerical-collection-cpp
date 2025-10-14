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
 * \brief Definition of internal functions to calculate inverse trigonometric
 * functions for quad class.
 */
#pragma once

#include <cmath>
#include <limits>

#include "num_collect/multi_double/impl/quad_internal_constants.h"
#include "num_collect/multi_double/impl/quad_trigonometric_impl.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate asin function using Newton method.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function assumes that the input is in the range
 * \f$ [-\sqrt{2}/2, \sqrt{2}/2] \f$.
 *
 * This function uses Newton method for
 * \f$ f(y) = \cos(y) - x \f$,
 * starting from the value of acos function of double.
 */
inline auto asin_newton(quad x) noexcept -> quad {
    quad guess = quad(std::asin(x.high()));
    guess -= (sin_maclaurin(guess) - x) / cos_maclaurin(guess);
    return guess;
}

/*!
 * \brief Calculate acos function using Newton method.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function assumes that the input is in the range
 * \f$ [\sqrt{2}/2, 1] \f$.
 *
 * This function uses Newton method for
 * \f$ f(y) = \cos(y) - x \f$,
 * starting from the value of acos function of double.
 */
inline auto acos_newton(quad x) noexcept -> quad {
    quad guess = quad(std::acos(x.high()));
    guess += (cos_maclaurin(guess) - x) / sin_maclaurin(guess);
    return guess;
}

/*!
 * \brief Calculate atan function using Newton method.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function assumes that the input is in the range
 * \f$ [-1, 1] \f$.
 *
 * This function uses Newton method for
 * \f$ f(y) = \tan(y) - x \f$,
 * starting from the value of atan function of double.
 */
inline auto atan_newton(quad x) noexcept -> quad {
    quad guess = quad(std::atan(x.high()));
    const quad sin_guess = sin_maclaurin(guess);
    const quad cos_guess = cos_maclaurin(guess);
    guess -= cos_guess * (sin_guess - x * cos_guess);
    return guess;
}

/*!
 * \brief Calculate asin function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto asin_impl(quad x) noexcept -> quad {
    if (x < -1.0 || x > 1.0) {
        return quad(std::numeric_limits<double>::quiet_NaN());
    }
    if (x > sqrt2_inv_quad) {
        return pi_over_2_quad - acos_newton(x);
    }
    if (x < -sqrt2_inv_quad) {
        return -pi_over_2_quad + acos_newton(-x);
    }
    return asin_newton(x);
}

/*!
 * \brief Calculate acos function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto acos_impl(quad x) noexcept -> quad {
    if (x < -1.0 || x > 1.0) {
        return quad(std::numeric_limits<double>::quiet_NaN());
    }
    if (x > sqrt2_inv_quad) {
        return acos_newton(x);
    }
    if (x < -sqrt2_inv_quad) {
        return pi_quad - acos_newton(-x);
    }
    return pi_over_2_quad - asin_newton(x);
}

/*!
 * \brief Calculate atan function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto atan_impl(quad x) noexcept -> quad {
    if (std::isinf(x.high())) {
        if (x.high() > 0.0) {
            return pi_over_2_quad;
        }
        return -pi_over_2_quad;
    }
    if (x > 1.0) {
        return pi_over_2_quad - atan_newton(1.0 / x);
    }
    if (x < -1.0) {
        return -pi_over_2_quad - atan_newton(1.0 / x);
    }
    return atan_newton(x);
}

/*!
 * \brief Calculate atan2 function.
 *
 * \param[in] y Input value.
 * \param[in] x Input value.
 * \return Result.
 *
 * This function returns NaN if both inputs are zero or
 * either input is infinite or NaN.
 */
inline auto atan2_impl(quad y, quad x) noexcept -> quad {
    if ((x.high() == 0.0 && y.high() == 0.0) || !std::isfinite(x.high()) ||
        !std::isfinite(y.high())) {
        return quad(std::numeric_limits<double>::quiet_NaN());
    }
    if (x > y) {
        if (x > -y) {
            return atan_newton(y / x);
        }
        return -pi_over_2_quad - atan_newton(x / y);
    }
    if (x > -y) {
        return pi_over_2_quad - atan_newton(x / y);
    }
    if (y >= 0.0) {
        return pi_quad + atan_newton(y / x);
    }
    return -pi_quad + atan_newton(y / x);
}

}  // namespace num_collect::multi_double::impl
