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
 * \brief Definition of dense_vector concept.
 */
#pragma once

#include "num_collect/base/concepts/dense_matrix.h"  // IWYU pragma: keep

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept of Eigen's dense vectors.
 *
 * \tparam T Type.
 */
template <typename T>
concept dense_vector = dense_matrix<T> && requires {
    T::ColsAtCompileTime;
    requires T::ColsAtCompileTime == 1;
};

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
