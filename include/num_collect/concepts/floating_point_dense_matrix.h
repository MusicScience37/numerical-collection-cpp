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
 * \brief Definition of floating_point_dense_matrix concept.
 */
#pragma once

#include "num_collect/concepts/dense_matrix.h"
#include "num_collect/concepts/floating_point.h"

namespace num_collect::concepts {

/*!
 * \brief Concept of Eigen's dense matrices with floating-point values.
 *
 * \tparam T Type.
 */
template <typename T>
concept floating_point_dense_matrix = dense_matrix<T> && requires {
    typename T::Scalar;
    requires floating_point<typename T::Scalar>;
};

}  // namespace num_collect::concepts
