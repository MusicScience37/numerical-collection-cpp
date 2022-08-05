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

#include <utility>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/formula.h"  // IWYU pragma: keep

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of embedded formulas.
 *
 * \tparam T Type.
 */
template <typename T>
concept embedded_formula = formula<T> && requires(T& obj) {
    {obj.step_embedded(std::declval<typename T::scalar_type>() /*time*/,
        std::declval<typename T::scalar_type>() /*step_size*/,
        std::declval<typename T::variable_type>() /*current*/,
        std::declval<typename T::variable_type&>() /*estimate*/,
        std::declval<typename T::variable_type&>() /*error*/)};
};

}  // namespace num_collect::ode::concepts
