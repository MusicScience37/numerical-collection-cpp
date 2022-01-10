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

#include <type_traits>

#include <Eigen/Core>

namespace num_collect::concepts {

/*!
 * \brief Concept of Eigen's matrices.
 *
 * \tparam T Type.
 */
template <typename T>
concept dense_matrix = std::is_base_of_v<Eigen::MatrixBase<T>, T>;

}  // namespace num_collect::concepts
