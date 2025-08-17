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
 * \brief Definition of getter_of concept.
 */
#pragma once

#include "num_collect/base/concepts/decayed_to.h"

namespace num_collect::logging::concepts {

/*!
 * \brief Concept of getter functions.
 *
 * \tparam Func Type of the function.
 * \tparam Value Type of returned values.
 */
template <typename Func, typename Value>
concept getter_of = requires(const Func& func) {
    { func() } -> base::concepts::decayed_to<Value>;
};

}  // namespace num_collect::logging::concepts
