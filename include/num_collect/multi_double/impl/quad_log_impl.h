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
 * \brief Definition of log_impl function for quad class.
 */
#pragma once

#include <cmath>

#include "num_collect/multi_double/impl/quad_exp_impl.h"
#include "num_collect/multi_double/impl/quad_internal_constants.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate natural logarithm \f$ \log(x) \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function uses Newton method for
 * \f$ f(y) = e^y - x \f$,
 * Starting from the value of log function of double.
 */
inline auto log_impl(quad x) noexcept -> quad {
    quad guess = quad(std::log(x.high()));
    if (!std::isfinite(guess.high())) {
        return guess;
    }
    guess += x * exp_impl(-guess) - quad(1.0);
    return guess;
}

/*!
 * \brief Calculate natural logarithm \f$ \log(1 + x) \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This function uses Newton method for
 * \f$ f(y) = e^y - x - 1 \f$,
 * Starting from the value of log function of double.
 * However, for not small values, this function uses log_impl function.
 */
inline auto log1p_impl(quad x) noexcept -> quad {
    constexpr double upper_threshold = 1.36e-3;    // expm1(1.36e-3)
    constexpr double lower_threshold = -1.359e-3;  // expm1(-1.36e-3)
    if (x.high() < lower_threshold || upper_threshold < x.high()) {
        return log_impl(quad(1.0) + x);
    }
    // special implementation
    quad guess = quad(std::log1p(x.high()));
    const quad exp_neg_guess = expm1_maclaurin_series(-guess);
    guess += x * (exp_neg_guess + quad(1.0)) + exp_neg_guess;
    return guess;
}

/*!
 * \brief Calculate common logarithm \f$ \log_{10}(x) \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto log10_impl(quad x) noexcept -> quad {
    const auto log_value = log_impl(x);
    if (!std::isfinite(log_value.high())) {
        return log_value;
    }
    return log_value * log10_inv_quad;
}

}  // namespace num_collect::multi_double::impl
