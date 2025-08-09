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
 * \brief Definition of dense_matrix concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include <Eigen/Core>

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept to check if T is an Eigen's dense matrix.
 *
 * \tparam T Type to check.
 *
 * This checks that T is derived from Eigen::MatrixBase.
 */
template <typename T>
concept dense_matrix = std::is_base_of_v<Eigen::MatrixBase<T>, T>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
