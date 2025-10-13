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

#include <concepts>

#include "num_collect/multi_double/impl/quad_exp_impl.h"
#include "num_collect/multi_double/impl/quad_log_impl.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/util/assert.h"

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

//! Maximum exponent for pow_positive_int_impl function.
constexpr unsigned int max_exponent_for_positive_int_impl = 1024U;

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`
 * when the exponent is a non-large positive integer.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 */
inline auto pow_positive_int_impl(quad base, unsigned int exponent) noexcept
    -> quad {
    NUM_COLLECT_DEBUG_ASSERT(exponent <= max_exponent_for_positive_int_impl);
    unsigned int current_exponent = 1U;
    quad current_power = base;
    quad result = 1.0;
    {
        if ((exponent & current_exponent) != 0U) {
            result *= current_power;
        }
        current_exponent <<= 1U;
    }
    while (current_exponent <= max_exponent_for_positive_int_impl) {
        current_power *= current_power;
        if ((exponent & current_exponent) != 0U) {
            result *= current_power;
        }
        current_exponent <<= 1U;
    }
    return result;
}

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 */
inline auto pow_impl(quad base, quad exponent) noexcept -> quad {
    return pow_general_impl(base, exponent);
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
inline auto pow_impl(quad base, Exponent exponent) noexcept -> quad {
    if (exponent <= max_exponent_for_positive_int_impl) {
        return pow_positive_int_impl(base, static_cast<unsigned int>(exponent));
    }
    return pow_general_impl(base, static_cast<double>(exponent));
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
inline auto pow_impl(quad base, Exponent exponent) noexcept -> quad {
    if (0 <= exponent &&
        exponent <= static_cast<Exponent>(max_exponent_for_positive_int_impl)) {
        return pow_positive_int_impl(base, static_cast<unsigned int>(exponent));
    }
    if (-static_cast<Exponent>(max_exponent_for_positive_int_impl) <=
            exponent &&
        exponent < 0) {
        return 1.0 /
            pow_positive_int_impl(base, static_cast<unsigned int>(-exponent));
    }
    return pow_general_impl(base, static_cast<double>(exponent));
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
    return pow_general_impl(base, static_cast<double>(exponent));
}

}  // namespace num_collect::multi_double::impl
