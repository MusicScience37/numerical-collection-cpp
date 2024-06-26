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
 * \brief Definition of single_variate_differentiable_function concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/roots/concepts/differentiable_function.h"

namespace num_collect::roots::concepts {

/*!
 * \brief Concept of single-variate differentiable functions for root-finding.
 *
 * \tparam T Type.
 */
template <typename T>
concept single_variate_differentiable_function = differentiable_function<T> &&
    base::concepts::real_scalar<typename T::variable_type> &&
    std::is_same_v<typename T::variable_type, typename T::jacobian_type>;

}  // namespace num_collect::roots::concepts
