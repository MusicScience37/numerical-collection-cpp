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
 * \brief Definition of multi_variate_differentiable_problem concept.
 */
#pragma once

#include "num_collect/ode/concepts/differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/multi_variate_problem.h"  // IWYU pragma: keep

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of problems of multi-variate differentiable ordinary
 * differential equations.
 *
 * \tparam T Type.
 */
template <typename T>
concept multi_variate_differentiable_problem =
    differentiable_problem<T> && multi_variate_problem<T>;

}  // namespace num_collect::ode::concepts
