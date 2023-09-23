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
 * \brief Definition of comparator concept.
 */
#pragma once

#include "num_collect/base/concepts/decayed_to.h"  // IWYU pragma: keep

namespace num_collect::util::concepts {

/*!
 * \brief Concept of classes to compare two values.
 *
 * \tparam C Type of class to compare two values.
 * \tparam T Type of the first argument.
 * \tparam U Type of the second argument.
 */
template <typename C, typename T, typename U>
concept comparator = requires(const C& c, const T& t, const U& u) {
    { c(t, u) } -> base::concepts::decayed_to<bool>;
};

}  // namespace num_collect::util::concepts
