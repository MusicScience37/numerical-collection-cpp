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
 * \brief Definition of real_scalar_dense_vector concept.
 */
#pragma once

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/real_scalar_dense_matrix.h"

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept to check if T is an Eigen's dense vector with real scalars.
 *
 * \tparam T Type to check.
 */
template <typename T>
concept real_scalar_dense_vector =
    real_scalar_dense_matrix<T> && dense_vector<T>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
