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
 * \brief Definition of reference_of concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept of references.
 *
 * \tparam T Type.
 * \tparam U Type.
 */
template <typename T, typename U>
concept reference_of = std::is_same_v<T, U&>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
