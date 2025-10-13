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
 * \brief Definition of internal functions to calculate hyperbolic functions for
 * quad class.
 */
#pragma once

#include <limits>

#include "num_collect/multi_double/impl/quad_exp_impl.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate sinh function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto sinh_impl(quad x) noexcept -> quad {
    constexpr double threshold = 1.0;
    if (-threshold < x.high() && x.high() < threshold) {
        const quad expm1_x = expm1_impl(x);
        const quad expm1_neg_x = -expm1_x / (expm1_x + 1.0);
        return (expm1_x - expm1_neg_x) * 0.5;  // NOLINT(*-magic-numbers)
    }
    const quad exp_x = exp_impl(x);
    if (!std::isfinite(exp_x.high())) {
        return exp_x;
    }
    if (exp_x.high() == 0.0) {
        return quad(-std::numeric_limits<double>::infinity());
    }
    return (exp_x - 1.0 / exp_x) * 0.5;  // NOLINT(*-magic-numbers)
}

/*!
 * \brief Calculate cosh function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto cosh_impl(quad x) noexcept -> quad {
    const quad exp_x = exp_impl(x);
    if (!std::isfinite(exp_x.high())) {
        return exp_x;
    }
    if (exp_x.high() == 0.0) {
        return quad(std::numeric_limits<double>::infinity());
    }
    return (exp_x + 1.0 / exp_x) * 0.5;  // NOLINT(*-magic-numbers)
}

/*!
 * \brief Calculate tanh function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto tanh_impl(quad x) noexcept -> quad {
    if (x.high() > 0.0) {
        const quad expm1_neg_2x = expm1_impl(-2.0 * x);
        return -expm1_neg_2x / (expm1_neg_2x + 2.0);  // NOLINT(*-magic-numbers)
    }
    const quad expm1_2x = expm1_impl(2.0 * x);
    return expm1_2x / (expm1_2x + 2.0);  // NOLINT(*-magic-numbers)
}

}  // namespace num_collect::multi_double::impl
