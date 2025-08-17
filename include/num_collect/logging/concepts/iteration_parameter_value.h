/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of iteration_parameter_value concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/decayed_to.h"

namespace num_collect::logging::concepts {

/*!
 * \brief Concept of classes used to get parameters values in iterations.
 *
 * \tparam T Type.
 * \tparam Algorithm Type of the algorithm.
 * \tparam Value Type of values.
 *
 * \note For APIs of functions, see implementations, for example
 * \ref num_collect::logging::iterations::variable_iteration_parameter_value
 * class.
 */
template <typename T, typename Algorithm, typename Value>
concept iteration_parameter_value = requires(const T& obj) {
    requires std::is_nothrow_move_constructible_v<T>;

    { obj.get() } -> base::concepts::decayed_to<Value>;

    requires requires(const Algorithm* algorithm) {
        { obj.get(algorithm) } -> base::concepts::decayed_to<Value>;
    };
};

}  // namespace num_collect::logging::concepts
