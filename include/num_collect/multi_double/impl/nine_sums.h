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
 * \brief Definitions of functions to add nine double numbers.
 */
#pragma once

#include <tuple>

#include "num_collect/multi_double/impl/basic_operations.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate sum of nine double numbers to get two terms
 * \cite Hida2000.
 *
 * \param[in] x1 A number.
 * \param[in] x2 A number.
 * \param[in] x3 A number.
 * \param[in] x4 A number.
 * \param[in] x5 A number.
 * \param[in] x6 A number.
 * \param[in] x7 A number.
 * \param[in] x8 A number.
 * \param[in] x9 A number.
 * \return Two terms representing the sum of arguments.
 * The first value is 0th order term,
 * and the second value is 1st order term.
 *
 * This function has no assumption of the magnitudes of the arguments.
 */
constexpr auto nine_to_two_sum(double x1, double x2, double x3, double x4,
    double x5, double x6, double x7, double x8, double x9)
    -> std::tuple<double, double> {
    // First calculate sums of two numbers to create four quad numbers.
    const auto [q1_high, q1_low] = two_sum(x1, x2);
    const auto q1 = quad(q1_high, q1_low);
    const auto [q2_high, q2_low] = two_sum(x3, x4);
    const auto q2 = quad(q2_high, q2_low);
    const auto [q3_high, q3_low] = two_sum(x5, x6);
    const auto q3 = quad(q3_high, q3_low);
    const auto [q4_high, q4_low] = two_sum(x7, x8);
    const auto q4 = quad(q4_high, q4_low);

    // Second calculate sum of four quad numbers.
    const quad s12 = q1 + q2;
    const quad s34 = q3 + q4;
    const quad s = s12 + s34;

    // Finally add the last number.
    auto [r, e] = two_sum(s.high(), x9);
    e += s.low();
    return {r, e};
}

}  // namespace num_collect::multi_double::impl
