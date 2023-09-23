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
 * \brief Definition of get_least_known_order function.
 */
#pragma once

#include "num_collect/base/concepts/decayed_to.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/formula.h"  // IWYU pragma: keep

namespace num_collect::ode::impl {

/*!
 * \brief Get the least known order of a formula.
 *
 * \tparam Formula Type of the formula.
 * \return Order.
 */
template <concepts::formula Formula>
constexpr auto get_least_known_order() -> index_type {
    if constexpr (requires() {
                      {
                          Formula::lesser_order
                      } -> base::concepts::decayed_to<index_type>;
                  }) {
        return Formula::lesser_order;
    } else {
        return Formula::order;
    }
}

}  // namespace num_collect::ode::impl
