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
 * \brief Definition of box_constrained_optimizer concept.
 */
#pragma once

#include "num_collect/opt/concepts/optimizer.h"

namespace num_collect::opt::concepts {

/*!
 * \brief Concept of optimizers with box constraints.
 *
 * \tparam T Type.
 */
template <typename T>
concept box_constrained_optimizer = optimizer<T>&&
    requires()
{
    requires requires(T& solver, const typename T::variable_type& min_var,
        const typename T::variable_type& max_var) {
        { solver.init(min_var, max_var) };
    };
};

}  // namespace num_collect::opt::concepts
