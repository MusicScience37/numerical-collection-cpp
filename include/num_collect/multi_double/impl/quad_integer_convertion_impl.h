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
 * \brief Definitions of functions to convert quad class to integers.
 */
#pragma once

#include <cmath>

#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculates floor function.
 *
 * \param[in] x Value.
 * \return Floor value.
 */
inline auto floor_impl(quad x) noexcept -> quad {
    double high_int{};
    double high_frac = std::modf(x.high(), &high_int);
    double low_int{};
    double low_frac = std::modf(x.low(), &low_int);
    return quad(high_int, low_int) + std::floor(high_frac + low_frac);
}

}  // namespace num_collect::multi_double::impl
