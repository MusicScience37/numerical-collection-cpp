/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief declaration and implementation of basic operations in multi-double
 * calculations
 */
#pragma once

#include <tuple>

namespace num_collect::multi_double::impl {

/*!
 * \brief calculate sum of a and b, and error of the sum
 *        on the condition that $|a| \ge |b|$
 *
 * \param[in] a a number
 * \param[in] b a number
 * \param[out] s sum of a and b
 * \param[out] e error of sum
 */
inline void quick_two_sum(double a, double b, double& s, double& e) {
    s = a + b;
    e = b - (s - a);
}

/*!
 * \brief calculate sum of a and b, and error of the sum
 *
 * \param[in] a a number
 * \param[in] b a number
 * \param[out] s sum of a and b
 * \param[out] e error of sum
 */
inline void two_sum(double a, double b, double& s, double& e) {
    s = a + b;
    double v = s - a;
    e = (a - (s - v)) + (b - v);
}

}  // namespace num_collect::multi_double::impl
