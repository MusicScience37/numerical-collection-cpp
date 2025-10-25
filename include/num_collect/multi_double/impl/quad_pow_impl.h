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
 * \brief Definition of internal functions to calculate pow function of quad
 * class.
 */
#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <type_traits>

#include "num_collect/multi_double/impl/quad_exp_impl.h"
#include "num_collect/multi_double/impl/quad_integer_convertion_impl.h"
#include "num_collect/multi_double/impl/quad_log_impl.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`
 * using exp and log functions.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 *
 * This function can handle only positive base values.
 */
inline auto pow_general_impl(quad base, quad exponent) noexcept -> quad {
    return exp_impl(log_impl(base) * exponent);
}

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`
 * using exp and log functions.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 *
 * This function can handle only positive base values.
 */
inline auto pow_general_impl(quad base, double exponent) noexcept -> quad {
    return exp_impl(log_impl(base) * exponent);
}

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`
 * when the exponent is a non-large positive integer.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 */
template <std::unsigned_integral Exponent>
constexpr auto pow_positive_int_impl(quad base, Exponent exponent) noexcept
    -> quad {
    quad result = quad(1.0);
    if (exponent == 0) {
        return result;
    }
    Exponent remaining_exponent = exponent;
    quad current_power = base;
    {
        // First time.
        if ((remaining_exponent & static_cast<Exponent>(1)) == 1) {
            result *= current_power;
        }
        remaining_exponent >>= 1U;
    }
    while (remaining_exponent > 0) {
        current_power *= current_power;
        if ((remaining_exponent & static_cast<Exponent>(1)) == 1) {
            result *= current_power;
        }
        remaining_exponent >>= 1U;
    }
    return result;
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
    requires concepts::implicitly_convertible_to<Exponent, double> &&
    std::unsigned_integral<Exponent>
constexpr auto pow_impl(quad base, Exponent exponent) noexcept -> quad {
    return pow_positive_int_impl(base, exponent);
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
    requires concepts::implicitly_convertible_to<Exponent, double> &&
    std::signed_integral<Exponent>
constexpr auto pow_impl(quad base, Exponent exponent) noexcept -> quad {
    if (exponent >= 0) {
        return pow_positive_int_impl(
            base, static_cast<std::make_unsigned_t<Exponent>>(exponent));
    }
    return 1.0 /
        pow_positive_int_impl(
            base, static_cast<std::make_unsigned_t<Exponent>>(-exponent));
}

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 */
inline auto pow_impl(quad base, quad exponent) noexcept -> quad {
    constexpr double integer_upper_limit = 0x1.0p+53;
    constexpr double integer_lower_limit = -0x1.0p+53;
    if (exponent <= integer_lower_limit || integer_upper_limit <= exponent) {
        return pow_general_impl(base, exponent);
    }
    const quad integer_part = floor_impl(exponent + 0.5);
    const quad fractional_part = exponent - integer_part;
    return pow_impl(base, static_cast<std::int64_t>(integer_part.high())) *
        pow_general_impl(base, fractional_part);
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
    requires concepts::implicitly_convertible_to<Exponent, double> &&
    (!std::integral<Exponent>)
inline auto pow_impl(quad base, Exponent exponent) noexcept -> quad {
    constexpr auto integer_upper_limit = static_cast<Exponent>(0x1.0p+53);
    constexpr auto integer_lower_limit = static_cast<Exponent>(-0x1.0p+53);
    if (exponent <= integer_lower_limit || integer_upper_limit <= exponent) {
        return pow_general_impl(base, exponent);
    }
    const Exponent integer_part =
        std::floor(exponent + static_cast<Exponent>(0.5));
    const Exponent fractional_part = exponent - integer_part;
    return pow_impl(base, static_cast<std::int64_t>(integer_part)) *
        pow_general_impl(base, static_cast<double>(fractional_part));
}

}  // namespace num_collect::multi_double::impl
