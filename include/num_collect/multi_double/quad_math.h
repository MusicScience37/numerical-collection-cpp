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
 * \brief Definition of mathematical functions of quad class.
 */
#pragma once

#include <cmath>

#include "num_collect/multi_double/impl/basic_operations.h"
#include "num_collect/multi_double/impl/quad_exp_impl.h"
#include "num_collect/multi_double/impl/quad_hyperbolic_impl.h"
#include "num_collect/multi_double/impl/quad_integer_convertion_impl.h"
#include "num_collect/multi_double/impl/quad_inv_trigonometric_impl.h"
#include "num_collect/multi_double/impl/quad_log_impl.h"
#include "num_collect/multi_double/impl/quad_pow_impl.h"
#include "num_collect/multi_double/impl/quad_trigonometric_impl.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double {

/*!
 * \brief Get the absolute value of a number.
 *
 * \param[in] value Input number.
 * \return Absolute value of the input number.
 */
inline auto abs(quad value) noexcept -> quad {
    if (value < quad(0.0)) {
        return -value;
    }
    return value;
}

/*!
 * \brief Calculate the square root of a number.
 *
 * \param[in] value Input number.
 * \return Square root of the input number.
 *
 * If the input number is negative, the result is unspecified.
 */
inline auto sqrt(quad value) noexcept -> quad {
    if (value == quad(0.0)) {
        return quad(0.0);
    }
    const double approx = std::sqrt(value.high());
    const auto [approx_square_high, approx_square_low] =
        impl::two_prod(approx, approx);
    const auto remaining =
        ((value.high() - approx_square_high) - approx_square_low) + value.low();
    const double correction = 0.5 * remaining / approx;
    const auto [result_high, result_low] =
        impl::quick_two_sum(approx, correction);
    return quad(result_high, result_low);
}

/*!
 * \brief Calculate exponential \f$ e^x \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto exp(quad x) noexcept -> quad { return impl::exp_impl(x); }

/*!
 * \brief Calculate exponential minus one \f$ e^x - 1 \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto expm1(quad x) noexcept -> quad { return impl::expm1_impl(x); }

/*!
 * \brief Calculate natural logarithm \f$ \log(x) \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto log(quad x) noexcept -> quad { return impl::log_impl(x); }

/*!
 * \brief Calculate natural logarithm \f$ \log(1 + x) \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto log1p(quad x) noexcept -> quad { return impl::log1p_impl(x); }

/*!
 * \brief Calculate common logarithm \f$ \log_{10}(x) \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto log10(quad x) noexcept -> quad { return impl::log10_impl(x); }

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 */
inline auto pow(quad base, quad exponent) noexcept -> quad {
    return impl::pow_impl(base, exponent);
}

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`.
 *
 * \tparam Exponent Type of the exponent value.
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 */
template <typename Exponent>
    requires concepts::implicitly_convertible_to<Exponent, double>
inline auto pow(quad base, Exponent exponent) noexcept -> quad {
    return impl::pow_impl(base, exponent);
}

/*!
 * \brief Calculate sin function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto sin(quad x) noexcept -> quad { return impl::sin_impl(x); }

/*!
 * \brief Calculate cos function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto cos(quad x) noexcept -> quad { return impl::cos_impl(x); }

/*!
 * \brief Calculate tan function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto tan(quad x) noexcept -> quad { return impl::tan_impl(x); }

/*!
 * \brief Calculate asin function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto asin(quad x) noexcept -> quad { return impl::asin_impl(x); }

/*!
 * \brief Calculate acos function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto acos(quad x) noexcept -> quad { return impl::acos_impl(x); }

/*!
 * \brief Calculate atan function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto atan(quad x) noexcept -> quad { return impl::atan_impl(x); }

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
inline auto atan2(quad y, quad x) noexcept -> quad {
    return impl::atan2_impl(y, x);
}

/*!
 * \brief Calculate sinh function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto sinh(quad x) noexcept -> quad { return impl::sinh_impl(x); }

/*!
 * \brief Calculate cosh function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto cosh(quad x) noexcept -> quad { return impl::cosh_impl(x); }

/*!
 * \brief Calculate tanh function.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto tanh(quad x) noexcept -> quad { return impl::tanh_impl(x); }

/*!
 * \brief Calculate floor function.
 *
 * \param[in] x Value.
 * \return Floor value.
 */
inline auto floor(quad x) noexcept -> quad { return impl::floor_impl(x); }

/*!
 * \brief Calculate ceil function.
 *
 * \param[in] x Value.
 * \return Ceil value.
 */
inline auto ceil(quad x) noexcept -> quad { return impl::ceil_impl(x); }

/*!
 * \brief Truncate to integer.
 *
 * \param[in] x Value.
 * \return Truncated value.
 */
inline auto trunc(quad x) noexcept -> quad { return impl::trunc_impl(x); }

/*!
 * \brief Round to nearest integer.
 *
 * \param[in] x Value.
 * \return Rounded value.
 */
inline auto round(quad x) noexcept -> quad { return impl::round_impl(x); }

}  // namespace num_collect::multi_double
