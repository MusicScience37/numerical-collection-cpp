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
 * \brief Definition of function concept.
 */
#pragma once

#include <utility>

#include "num_collect/base/concepts/const_reference_of.h"

namespace num_collect::roots::concepts {

/*!
 * \brief Concept of functions for root-finding.
 *
 * \tparam T Type.
 */
template <typename T>
concept function = requires(const T& const_obj, T& obj) {
    typename T::variable_type;

    {obj.evaluate_on(std::declval<typename T::variable_type>())};
    {
        const_obj.value()
        } -> base::concepts::const_reference_of<typename T::variable_type>;
};

}  // namespace num_collect::roots::concepts
