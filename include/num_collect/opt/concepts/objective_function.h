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
 * \brief Definition of objective_function concept.
 */
#pragma once

#include "num_collect/base/concepts/const_reference_of.h"
#include "num_collect/base/concepts/real_scalar.h"

namespace num_collect::opt::concepts {

/*!
 * \brief Concept of objective functions in optimization.
 *
 * \tparam T Type.
 */
template <typename T>
concept objective_function = requires() {
    typename T::variable_type;
    typename T::value_type;

    requires base::concepts::real_scalar<typename T::value_type>;

    requires requires(T& obj, const typename T::variable_type& var) {
        { obj.evaluate_on(var) };
    };

    requires requires(const T& obj) {
        {
            obj.value()
        } -> base::concepts::const_reference_of<typename T::value_type>;
    };
};

}  // namespace num_collect::opt::concepts
