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
 * \brief Definition of distance_function concept.
 */
#pragma once

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar_dense_vector.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/same_as.h"  // IWYU pragma: keep

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of distance functions.
 *
 * \tparam T Type.
 */
template <typename T>
concept distance_function = requires() {
    typename T::variable_type;
    typename T::value_type;

    requires base::concepts::real_scalar<typename T::value_type>;

    requires requires(const T& obj, const typename T::variable_type& var1,
        const typename T::variable_type& var2) {
        { obj(var1, var2) } -> base::concepts::same_as<typename T::value_type>;
    };
};

}  // namespace num_collect::rbf::concepts
