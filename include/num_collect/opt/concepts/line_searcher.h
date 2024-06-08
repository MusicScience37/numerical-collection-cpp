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
 * \brief Definition of line_searcher concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/const_reference_of.h"
#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/base/concepts/reference_of.h"
#include "num_collect/base/index_type.h"
#include "num_collect/opt/concepts/differentiable_objective_function.h"

namespace num_collect::opt::concepts {

/*!
 * \brief Concept of objects to perform line search in optimization.
 *
 * \tparam T Type.
 */
template <typename T>
concept line_searcher = requires() {
    typename T::objective_function_type;
    requires differentiable_objective_function<
        typename T::objective_function_type>;

    typename T::variable_type;
    requires std::is_same_v<typename T::variable_type,
        typename T::objective_function_type::variable_type>;

    typename T::value_type;
    requires std::is_same_v<typename T::value_type,
        typename T::objective_function_type::value_type>;

    requires requires(T& obj, const typename T::variable_type& init_variable) {
        { obj.init(init_variable) };
    };

    requires requires(T& obj, const typename T::variable_type& direction) {
        { obj.search(direction) };
    };

    requires requires(T& obj) {
        {
            obj.obj_fun()
        } -> base::concepts::reference_of<typename T::objective_function_type>;
    };

    requires requires(const T& obj) {
        {
            obj.obj_fun()
        } -> base::concepts::const_reference_of<
              typename T::objective_function_type>;

        {
            obj.opt_variable()
        } -> base::concepts::const_reference_of<typename T::variable_type>;

        {
            obj.opt_value()
        } -> base::concepts::const_reference_of<typename T::value_type>;

        {
            obj.gradient()
        } -> base::concepts::const_reference_of<typename T::variable_type>;

        {
            obj.evaluations()
        } -> base::concepts::implicitly_convertible_to<index_type>;
    };
};

}  // namespace num_collect::opt::concepts
