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
 * \brief Definition of euclidean_distance class
 */
#pragma once

#include <type_traits>
#include <utility>

#include "num_collect/base/norm.h"

namespace num_collect::interp::kernel {

/*!
 * \brief Class to calculate Euclidean distance.
 *
 * \tparam Variable Type of Variables.
 */
template <typename Variable>
class euclidean_distance {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of values of distances.
    using value_type =
        std::decay_t<decltype(norm(std::declval<variable_type>()))>;

    /*!
     * \brief Calculate a distance.
     *
     * \param[in] var1 Variable.
     * \param[in] var2 Variable.
     * \return Distance between var1 and var2.
     */
    [[nodiscard]] auto operator()(const variable_type& var1,
        const variable_type& var2) const -> value_type {
        return norm(var1 - var2);
    }
};

}  // namespace num_collect::interp::kernel
