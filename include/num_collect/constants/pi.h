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
 * \brief Definition of pi.
 */
#pragma once

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief First 50 digits of pi.
 */
#define NUM_COLLECT_PI 3.14159265358979323846264338327950288419716939937510
#else
// NOLINTNEXTLINE
#define NUM_COLLECT_PI 3.14159265358979323846264338327950288419716939937510
#endif

namespace num_collect::constants {

/*!
 * \brief Value of pi.
 *
 * \tparam T Value type.
 */
template <typename T>
constexpr T pi = NUM_COLLECT_PI;

/*!
 * \brief Value of pi.
 */
constexpr double pid = pi<double>;

/*!
 * \brief Value of pi.
 */
constexpr float pif = pi<float>;

}  // namespace num_collect::constants
