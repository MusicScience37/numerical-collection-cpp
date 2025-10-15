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
 * \brief Definition of sqrt_impl function.
 */
#pragma once

#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate the square root of a number.
 *
 * \param[in] value Input number.
 * \return Square root of the input number.
 *
 * If the input number is negative, the result is unspecified.
 */
inline auto sqrt_impl(quad value) noexcept -> quad {
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

}  // namespace num_collect::multi_double::impl
