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
 * \brief Definitions of functions to add six double numbers.
 */
#pragma once

#include <tuple>

#include "num_collect/multi_double/impl/basic_operations.h"
#include "num_collect/multi_double/impl/three_sums.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate sum of six double numbers to get three terms
 * \cite Hida2000.
 *
 * \param[in] x1 A number.
 * \param[in] x2 A number.
 * \param[in] x3 A number.
 * \param[in] x4 A number.
 * \param[in] x5 A number.
 * \param[in] x6 A number.
 * \return Three terms representing the sum of arguments.
 * The first value is 0th order term,
 * the second value is 1st order term,
 * and the third value is 2nd order term.
 *
 * This function has no assumption of the magnitudes of the arguments.
 */
constexpr auto six_to_three_sum(double x1, double x2, double x3, double x4,
    double x5, double x6) -> std::tuple<double, double, double> {
    // Last digit of the variable names indicates the order.

    // First calculate sums of three numbers.
    const auto [s1_0, s1_1, s1_2] = three_to_three_sum(x1, x2, x3);
    const auto [s2_0, s2_1, s2_2] = three_to_three_sum(x4, x5, x6);

    // Then combine the sums.
    const auto [r_0, e0_1] = two_sum(s1_0, s2_0);
    const auto [t_1, e11_2] = two_sum(s1_1, s2_1);
    const auto [r_1, e12_2] = two_sum(t_1, e0_1);
    const double r_2 = s1_2 + s2_2 + e11_2 + e12_2;
    return {r_0, r_1, r_2};
}

}  // namespace num_collect::multi_double::impl
