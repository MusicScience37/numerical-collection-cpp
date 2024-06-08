/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of length_parameter_calculator concept.
 */
#pragma once

#include <vector>

#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/concepts/distance_function.h"

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of calculator of length parameters.
 *
 * \tparam T Type.
 */
template <typename T>
concept length_parameter_calculator = requires() {
    typename T::distance_function_type;
    typename T::variable_type;
    typename T::scalar_type;

    requires distance_function<typename T::distance_function_type>;
    requires base::concepts::real_scalar<typename T::scalar_type>;

    T();

    requires requires(T& obj,
        const std::vector<typename T::variable_type>& variables,
        const typename T::distance_function_type& distance_function) {
        obj.compute(variables, distance_function);
    };

    requires requires(const T& obj, index_type i) {
        {
            obj.length_parameter_at(i)
        } -> base::concepts::implicitly_convertible_to<typename T::scalar_type>;
    };

    requires requires(const T& obj) {
        {
            obj.scale()
        } -> base::concepts::implicitly_convertible_to<typename T::scalar_type>;
    };

    requires requires(
        T& obj, const typename T::scalar_type& value) { obj.scale(value); };
};

}  // namespace num_collect::rbf::concepts
