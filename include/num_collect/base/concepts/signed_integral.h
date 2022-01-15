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
 * \brief Definition of signed_integral concept.
 */
#pragma once

#include <type_traits>

#include "num_collect/base/concepts/integral.h"

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Class to check whether a type is signed.
 *
 * \tparam T Type to be checked.
 *
 * \note For user-defined types, write specializations of
 * this class.
 */
template <typename T>
struct is_signed : public std::is_signed<T> {};

/*!
 * \brief Get whether a type is signed.
 *
 * \tparam T Type to be checked.
 */
template <typename T>
constexpr bool is_signed_v = is_signed<T>::value;

/*!
 * \brief Concept of signed ingegers.
 *
 * \tparam T Type.
 */
template <typename T>
concept signed_integral = is_integral_v<T> && is_signed_v<T>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
