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
 * \brief Definition of container_compatible_range concept.
 */
#pragma once

#include <concepts>
#include <ranges>

namespace num_collect::util::concepts {

/*!
 * \brief Concept of ranges compatible with containers.
 *
 * \tparam Range Type of the range.
 * \tparam Value Type of values.
 */
template <typename Range, typename Value>
concept container_compatible_range = std::ranges::input_range<Range> &&
    std::convertible_to<std::ranges::range_reference_t<Range>, Value>;

}  // namespace num_collect::util::concepts
