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
 * \brief Definition of twice_differentiable_objective_function concept.
 */
#pragma once

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/opt/concepts/differentiable_objective_function.h"  // IWYU pragma: keep

namespace num_collect::opt::concepts {

/*!
 * \brief Concept of second-order differentiable objective functions in
 * optimization.
 *
 * \tparam T Type.
 */
template <typename T>
concept twice_differentiable_objective_function =
    differentiable_objective_function<T> && requires(const T& obj) {
    typename T::hessian_type;

    {
        obj.hessian()
        } -> base::concepts::const_reference_of<typename T::hessian_type>;
};

}  // namespace num_collect::opt::concepts
