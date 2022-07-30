/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of multi_variate_differentiable_function concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/real_scalar_dense_matrix.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar_dense_vector.h"  // IWYU pragma: keep
#include "num_collect/roots/concepts/differentiable_function.h"  // IWYU pragma: keep

namespace num_collect::roots::concepts {

/*!
 * \brief Concept of multi-variate differentiable functions for root-finding.
 *
 * \tparam T Type.
 */
template <typename T>
concept multi_variate_differentiable_function = differentiable_function<T> &&
    base::concepts::real_scalar_dense_vector<typename T::variable_type> &&
    base::concepts::real_scalar_dense_matrix<typename T::jacobian_type> &&
    std::is_same_v<typename T::variable_type::Scalar,
        typename T::jacobian_type::Scalar>;

}  // namespace num_collect::roots::concepts
