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
 * \brief Definition of embedded_formula concept.
 */
#pragma once

#include "num_collect/ode/concepts/formula.h"

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of embedded formulas.
 *
 * \tparam T Type.
 */
template <typename T>
concept embedded_formula = formula<T>&&
    requires()
{
    requires requires(T& obj, const typename T::scalar_type& time,
        const typename T::scalar_type& step_size,
        const typename T::variable_type& current,
        typename T::variable_type& estimate, typename T::variable_type& error) {
        obj.step_embedded(time, step_size, current, estimate, error);
    };
};

}  // namespace num_collect::ode::concepts
