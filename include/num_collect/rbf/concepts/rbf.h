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
 * \brief Definition of rbf concept.
 */
#pragma once

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/same_as.h"      // IWYU pragma: keep

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of RBFs.
 *
 * \tparam T Type.
 */
template <typename T>
concept rbf = requires() {
    typename T::scalar_type;

    requires base::concepts::real_scalar<typename T::scalar_type>;

    requires requires(
        const T& obj, const typename T::scalar_type& distance_rate) {
        {
            obj(distance_rate)
        } -> base::concepts::same_as<typename T::scalar_type>;
    };
};

}  // namespace num_collect::rbf::concepts
