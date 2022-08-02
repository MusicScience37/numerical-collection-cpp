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
 * \brief Definition of problem concept.
 */
#pragma once

#include <utility>

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/decayed_to.h"          // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar.h"         // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of problems of ordinary differential equations.
 *
 * \tparam T Type.
 */
template <typename T>
concept problem = requires(T& obj, const T& const_obj) {
    typename T::variable_type;
    typename T::scalar_type;

    requires base::concepts::real_scalar<typename T::scalar_type>;
    requires requires(
        typename T::variable_type & var, const typename T::scalar_type& coeff) {
        var = var + coeff * var;
    };

    { T::allowed_evaluations } -> base::concepts::decayed_to<evaluation_type>;
    requires T::allowed_evaluations.allows(evaluation_type{.diff_coeff = true});

    requires requires(const typename T::scalar_type& time,
        const typename T::variable_type& var, evaluation_type evaluations) {
        obj.evaluate_on(time, var, evaluations);
    };

    {
        const_obj.diff_coeff()
        } -> base::concepts::const_reference_of<typename T::variable_type>;
};

}  // namespace num_collect::ode::concepts
