/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of dense_vector_of concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/dense_vector.h"  // IWYU pragma: keep

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept of Eigen's dense vectors with scalars of the given type.
 *
 * \tparam T Type.
 * \tparam Scalar Type of the scalars.
 */
template <typename T, typename Scalar>
concept dense_vector_of =
    dense_vector<T> && requires {
                           typename T::Scalar;
                           requires std::is_same_v<typename T::Scalar, Scalar>;
                       };

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
