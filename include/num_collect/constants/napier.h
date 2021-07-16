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
 * \brief Definition of napier.
 */
#pragma once

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief First 50 digits of Napier's constant (or Euler's number).
 */
#define NUM_COLLECT_NAPIER 2.71828182845904523536028747135266249775724709369995
#else
// NOLINTNEXTLINE
#define NUM_COLLECT_NAPIER 2.71828182845904523536028747135266249775724709369995
#endif

namespace num_collect::constants {

/*!
 * \brief Value of Napier's constant (or Euler's number).
 *
 * \tparam T Value type.
 */
template <typename T>
constexpr T napier = static_cast<T>(NUM_COLLECT_NAPIER);

}  // namespace num_collect::constants
