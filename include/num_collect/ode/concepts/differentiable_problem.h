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
 * \brief Definition of differentiable_problem concept.
 */
#pragma once

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/problem.h"              // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of problems of differentiable ordinary differential equations.
 *
 * \tparam T Type.
 */
template <typename T>
concept differentiable_problem =
    problem<T> && requires(T& obj, const T& const_obj) {
        typename T::jacobian_type;

        requires requires(typename T::variable_type& var,
            const typename T::jacobian_type& coeff) {
            var = var + coeff * var;
        };

        requires T::allowed_evaluations.allows(
            evaluation_type{.jacobian = true});

        {
            const_obj.jacobian()
        } -> base::concepts::const_reference_of<typename T::jacobian_type>;
    };

}  // namespace num_collect::ode::concepts
