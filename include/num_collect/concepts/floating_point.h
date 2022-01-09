/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of floating_point concept.
 */
#pragma once

#include <type_traits>

namespace num_collect::concepts {

/*!
 * \brief Class to check whether a type is a floating-point value or a type
 * compatible with floating-point values.
 *
 * \tparam T Type to be checked.
 *
 * \note For user-defined floating-point-like values, write specializations of
 * this class.
 */
template <typename T>
struct is_floating_point : public std::is_floating_point<T> {};

/*!
 * \brief Get whether a type is a floating-point value or a type compatible with
 * floating-point values.
 *
 * \tparam T Type to be checked.
 */
template <typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

/*!
 * \brief Concept of floating-point values.
 *
 * \tparam T Type.
 */
template <typename T>
concept floating_point = is_floating_point_v<T>;

}  // namespace num_collect::concepts
