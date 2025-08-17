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
 * \brief Definition of optimizer concept.
 */
#pragma once

#include <concepts>

#include "num_collect/base/concepts/const_reference_of.h"
#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/base/concepts/iterative_solver.h"
#include "num_collect/base/index_type.h"
#include "num_collect/opt/concepts/objective_function.h"

namespace num_collect::opt::concepts {

/*!
 * \brief Concept of optimizers.
 *
 * \tparam T Type.
 */
template <typename T>
concept optimizer = base::concepts::iterative_solver<T> && requires() {
    typename T::objective_function_type;
    requires objective_function<typename T::objective_function_type>;

    typename T::variable_type;
    requires std::same_as<typename T::objective_function_type::variable_type,
        typename T::variable_type>;

    typename T::value_type;
    requires std::same_as<typename T::objective_function_type::value_type,
        typename T::value_type>;

    requires requires(const typename T::objective_function_type& obj_fun) {
        { T{obj_fun} };
    };

    requires requires(const T& obj) {
        {
            obj.opt_variable()
        } -> base::concepts::const_reference_of<typename T::variable_type>;

        {
            obj.opt_value()
        } -> base::concepts::const_reference_of<typename T::value_type>;

        {
            obj.iterations()
        } -> base::concepts::implicitly_convertible_to<index_type>;

        {
            obj.evaluations()
        } -> base::concepts::implicitly_convertible_to<index_type>;
    };
};

}  // namespace num_collect::opt::concepts
