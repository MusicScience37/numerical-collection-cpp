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
 * \brief Definition of rhs_comparable concept.
 */
#pragma once

#include "num_collect/util/concepts/comparable.h"

namespace num_collect::util::concepts {

/*!
 * \brief Concept of types comparable with `operator<` as the right-hand-side
 * object.
 *
 * \tparam T Right-hand-side type.
 * \tparam U Left-hand-side type.
 */
template <typename T, typename U>
concept rhs_less_than_comparable = less_than_comparable<U, T>;

/*!
 * \brief Concept of types comparable with `operator<=` as the right-hand-side
 * object.
 *
 * \tparam T Right-hand-side type.
 * \tparam U Left-hand-side type.
 */
template <typename T, typename U>
concept rhs_less_than_or_equal_to_comparable =
    less_than_or_equal_to_comparable<U, T>;

/*!
 * \brief Concept of types comparable with `operator>` as the right-hand-side
 * object.
 *
 * \tparam T Right-hand-side type.
 * \tparam U Left-hand-side type.
 */
template <typename T, typename U>
concept rhs_greater_than_comparable = greater_than_comparable<U, T>;

/*!
 * \brief Concept of types comparable with `operator>=` as the right-hand-side
 * object.
 *
 * \tparam T Right-hand-side type.
 * \tparam U Left-hand-side type.
 */
template <typename T, typename U>
concept rhs_greater_than_or_equal_to_comparable =
    greater_than_or_equal_to_comparable<U, T>;

/*!
 * \brief Concept of types comparable with `operator==` as the right-hand-side
 * object.
 *
 * \tparam T Right-hand-side type.
 * \tparam U Left-hand-side type.
 */
template <typename T, typename U>
concept rhs_equal_to_comparable = equal_to_comparable<U, T>;

/*!
 * \brief Concept of types comparable with `operator!=` as the right-hand-side
 * object.
 *
 * \tparam T Right-hand-side type.
 * \tparam U Left-hand-side type.
 */
template <typename T, typename U>
concept rhs_not_equal_to_comparable = not_equal_to_comparable<U, T>;

/*!
 * \brief Concept of types comparable as the right-hand-side object.
 *
 * \tparam T Right-hand-side type.
 * \tparam U Left-hand-side type.
 */
template <typename T, typename U>
concept rhs_comparable = comparable<U, T>;

}  // namespace num_collect::util::concepts
