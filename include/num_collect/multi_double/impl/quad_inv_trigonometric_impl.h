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
 * \brief Calculate asin function.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function uses Newton method for
 * \f$ f(y) = \sin(y) - x \f$,
 * starting from the value of asin function of double.
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

}  // namespace num_collect::multi_double::impl
