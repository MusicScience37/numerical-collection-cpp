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
 * \brief Definition of formula_solver concept.
 */
#pragma once

#include <utility>

#include "num_collect/base/concepts/const_reference_of.h"
#include "num_collect/base/concepts/reference_of.h"
#include "num_collect/ode/concepts/problem.h"

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of solvers of implicit formulas.
 *
 * \tparam T Type.
 */
template <typename T>
concept formula_solver = requires(T& obj, const T& const_obj) {
    typename T::problem_type;

    requires problem<typename T::problem_type>;

    { T(std::declval<typename T::problem_type>()) };

    { obj.problem() } -> base::concepts::reference_of<typename T::problem_type>;
    {
        const_obj.problem()
    } -> base::concepts::const_reference_of<typename T::problem_type>;
};

}  // namespace num_collect::ode::concepts
