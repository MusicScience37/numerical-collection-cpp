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
 * \brief Definition of comparable concept.
 */
#pragma once

#include "num_collect/base/concepts/decayed_to.h"

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept of comparable types with `operator<`.
 *
 * \tparam T Left-hand-side type.
 * \tparam U Right-hand-side type.
 */
template <typename T, typename U>
concept less_than_comparable = requires(const T& t, const U& u) {
    { t < u } -> decayed_to<bool>;
};

/*!
 * \brief Concept of comparable types with `operator<=`.
 *
 * \tparam T Left-hand-side type.
 * \tparam U Right-hand-side type.
 */
template <typename T, typename U>
concept less_than_or_equal_to_comparable = requires(const T& t, const U& u) {
    { t <= u } -> decayed_to<bool>;
};

/*!
 * \brief Concept of comparable types with `operator>`.
 *
 * \tparam T Left-hand-side type.
 * \tparam U Right-hand-side type.
 */
template <typename T, typename U>
concept greater_than_comparable = requires(const T& t, const U& u) {
    { t > u } -> decayed_to<bool>;
};

/*!
 * \brief Concept of comparable types with `operator>=`.
 *
 * \tparam T Left-hand-side type.
 * \tparam U Right-hand-side type.
 */
template <typename T, typename U>
concept greater_than_or_equal_to_comparable = requires(const T& t, const U& u) {
    { t >= u } -> decayed_to<bool>;
};

/*!
 * \brief Concept of comparable types with `operator==`.
 *
 * \tparam T Left-hand-side type.
 * \tparam U Right-hand-side type.
 */
template <typename T, typename U>
concept equal_to_comparable = requires(const T& t, const U& u) {
    { t == u } -> decayed_to<bool>;
};

/*!
 * \brief Concept of comparable types with `operator!=`.
 *
 * \tparam T Left-hand-side type.
 * \tparam U Right-hand-side type.
 */
template <typename T, typename U>
concept not_equal_to_comparable = requires(const T& t, const U& u) {
    { t != u } -> decayed_to<bool>;
};

/*!
 * \brief Concept of comparable types.
 *
 * \tparam T Left-hand-side type.
 * \tparam U Right-hand-side type.
 */
template <typename T, typename U>
concept comparable = less_than_comparable<T, U> &&
    less_than_or_equal_to_comparable<T, U> && greater_than_comparable<T, U> &&
    greater_than_or_equal_to_comparable<T, U> && equal_to_comparable<T, U> &&
    not_equal_to_comparable<T, U>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
