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
 * \brief Definition of internal functions to calculate inverse hyperbolic
 * functions for quad class.
 */
#pragma once

#include <limits>

#include "num_collect/multi_double/impl/quad_log_impl.h"
#include "num_collect/multi_double/impl/quad_root_of_one_plus_square.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate asinh function.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This calculates
 * \f[
 * \mathrm{asinh}(x) = \log(x + \sqrt{x^2 + 1})
 * \f]
 *
 * For \f$ |x| < 1 \f$, this uses
 * \f[
 * \mathrm{asinh}(x) = \log\left(x + \frac{x^2}{1 + \sqrt{x^2 + 1}} + 1 \right)
 * \f]
 * for \f$ x > 1 \f$, this uses
 * \f[
 * \mathrm{asinh}(x) = \log\left(x + x \sqrt{1 + \frac{1}{x^2}} \right)
 * \f]
 * and for \f$ x < -1 \f$, this uses
 * \f[
 * \mathrm{asinh}(x) = -\log\left(-x - x \sqrt{1 + \frac{1}{x^2}} \right)
 * \f]
 * to avoid cancellation and overflow.
 */
inline auto asinh_impl(quad x) noexcept -> quad {
    constexpr double threshold = 1.0;
    if (-threshold < x.high() && x.high() < threshold) {
        const quad root = root_of_one_plus_square(x);
        return log1p_impl(x + x * x / (1.0 + root));
    }
    if (x.high() > 0.0) {
        const quad root = root_of_one_plus_square(1.0 / x);
        return log_impl(x + x * root);
    }
    const quad root = root_of_one_plus_square(1.0 / x);
    return -log_impl(-x - x * root);
}

/*!
 * \brief Calculate acosh function.
 *
 * \param[in] x Input value.
 * \return Result.
 *
 * This calculates
 * \f[
 * \mathrm{acosh}(x) = \log(x + \sqrt{x^2 - 1})
 * \f]
 */
inline auto acosh_impl(quad x) noexcept -> quad {
    if (x < 1.0) {
        return quad(std::numeric_limits<double>::quiet_NaN());
    }
    return log_impl(x + sqrt_impl(x * x - 1.0));
}

}  // namespace num_collect::multi_double::impl
