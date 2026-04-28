/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of step_size_strategy concept.
 */
#pragma once

#include "num_collect/base/index_type.h"

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of classes of strategies to calculate next step sizes.
 *
 * \tparam T Type.
 */
template <typename T>
concept step_size_strategy = requires() {
    typename T::scalar_type;

    requires requires(index_type method_order) { T(method_order); };

    requires requires(T& obj) { obj.init(); };

    requires requires(T& obj, typename T::scalar_type& step_size,
        const typename T::scalar_type& error_norm) {
        obj.calc_next(step_size, error_norm);
    };
};

}  // namespace num_collect::ode::concepts
