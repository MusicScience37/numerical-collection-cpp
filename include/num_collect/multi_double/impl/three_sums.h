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
 * \brief Definitions of functions to add three double numbers.
 */
#pragma once

#include <tuple>

#include "num_collect/multi_double/impl/basic_operations.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate sum of three double numbers to get three terms
 * \cite Hida2000.
 *
 * \param[in] x A number.
 * \param[in] y A number.
 * \param[in] z A number.
 * \return Three terms representing the sum of x, y, and z.
 * The first value is 0th order term,
 * the second value is 1st order term,
 * and the third value is 2nd order term.
 *
 * This function has no assumption of the magnitudes of x, y, and z.
 */
constexpr auto three_to_three_sum(double x, double y, double z)
    -> std::tuple<double, double, double> {
    // Last digit of the variable names indicates the order.
    const auto [s_0, e1_1] = two_sum(x, y);
    const auto [r_0, e2_1] = two_sum(s_0, z);
    const auto [r_1, r_2] = two_sum(e1_1, e2_1);
    return {r_0, r_1, r_2};
}

/*!
 * \brief Calculate sum of three double numbers to get two terms
 * \cite Hida2000.
 *
 * \param[in] x A number.
 * \param[in] y A number.
 * \param[in] z A number.
 * \return Two terms representing the sum of x, y, and z.
 * The first value is 0th order term,
 * and the second value is 1st order term.
 *
 * This function has no assumption of the magnitudes of x, y, and z.
 */
constexpr auto three_to_two_sum(double x, double y, double z)
    -> std::tuple<double, double> {
    // Last digit of the variable names indicates the order.
    const auto [s_0, e1_1] = two_sum(x, y);
    const auto [r_0, e2_1] = two_sum(s_0, z);
    const double r_1 = e1_1 + e2_1;
    return {r_0, r_1};
}

}  // namespace num_collect::multi_double::impl
