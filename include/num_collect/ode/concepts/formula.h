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
 * \brief Definition of formula concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep
#include <utility>

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/decayed_to.h"          // IWYU pragma: keep
#include "num_collect/base/concepts/reference_of.h"        // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of formulas.
 *
 * \tparam T Type.
 */
template <typename T>
concept formula =
    requires(T& obj, const T& const_obj) {
        typename T::problem_type;
        typename T::variable_type;
        typename T::scalar_type;

        requires problem<typename T::problem_type>;
        requires std::is_same_v<typename T::variable_type,
            typename T::problem_type::variable_type>;
        requires std::is_same_v<typename T::scalar_type,
            typename T::problem_type::scalar_type>;

        { T::stages } -> base::concepts::decayed_to<index_type>;
        { T::order } -> base::concepts::decayed_to<index_type>;
        { T::log_tag } -> base::concepts::decayed_to<logging::log_tag_view>;

        { T(std::declval<typename T::problem_type>()) };

        {
            obj.step(std::declval<typename T::scalar_type>() /*time*/,
                std::declval<typename T::scalar_type>() /*step_size*/,
                std::declval<typename T::variable_type>() /*current*/,
                std::declval<typename T::variable_type&>() /*estimate*/)
        };

        {
            obj.problem()
            } -> base::concepts::reference_of<typename T::problem_type>;
        {
            const_obj.problem()
            } -> base::concepts::const_reference_of<typename T::problem_type>;
    };

}  // namespace num_collect::ode::concepts
