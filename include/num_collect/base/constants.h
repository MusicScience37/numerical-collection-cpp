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
 * \brief Definition of constants.
 */
#pragma once

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief First 50 digits of Napier's constant (or Euler's number).
 */
#define NUM_COLLECT_NAPIER 2.71828182845904523536028747135266249775724709369995
#else
#define NUM_COLLECT_NAPIER 2.71828182845904523536028747135266249775724709369995
#endif

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief First 50 digits of pi.
 */
#define NUM_COLLECT_PI 3.14159265358979323846264338327950288419716939937510
#else
#define NUM_COLLECT_PI 3.14159265358979323846264338327950288419716939937510
#endif

namespace num_collect {
inline namespace base {

/*!
 * \brief Value of Napier's constant (or Euler's number),
 * \f$ e = 2.7182818284590452 \ldots \f$.
 *
 * \tparam T Value type.
 */
template <typename T>
constexpr T napier = static_cast<T>(NUM_COLLECT_NAPIER);

/*!
 * \brief Value of pi, \f$ \pi = 3.141592653589793238462 \dots \f$.
 *
 * \tparam T Value type.
 */
template <typename T>
constexpr T pi = static_cast<T>(NUM_COLLECT_PI);

/*!
 * \brief Value of pi.
 */
constexpr double pid = pi<double>;

/*!
 * \brief Value of pi.
 */
constexpr float pif = pi<float>;

}  // namespace base
}  // namespace num_collect
