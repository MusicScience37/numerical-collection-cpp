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
 * \brief Definition of oct_renormalize function.
 */
#pragma once

#include <array>

#include "num_collect/multi_double/impl/basic_operations.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Renormalize an octuple-precision number.
 *
 * \param[in] inputs An array of 5 double-precision numbers.
 * \return An array of 4 double-precision numbers after renormalization.
 */
constexpr auto oct_renormalize(std::array<double, 5> inputs)
    -> std::array<double, 4> {
    double temp_high{};
    std::tie(temp_high, inputs[4]) = quick_two_sum(inputs[3], inputs[4]);
    std::tie(temp_high, inputs[3]) = quick_two_sum(inputs[2], temp_high);
    std::tie(temp_high, inputs[2]) = quick_two_sum(inputs[1], temp_high);
    std::tie(inputs[0], inputs[1]) = quick_two_sum(inputs[0], temp_high);

    std::array<double, 4> results{};
    double temp_low{};
    std::tie(results[0], temp_low) = quick_two_sum(inputs[0], inputs[1]);
    std::tie(results[1], temp_low) = quick_two_sum(temp_low, inputs[2]);
    std::tie(results[2], temp_low) = quick_two_sum(temp_low, inputs[3]);
    results[3] = temp_low + inputs[4];
    return results;
}

}  // namespace num_collect::multi_double::impl
