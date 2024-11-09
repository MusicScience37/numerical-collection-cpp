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
 * \brief Definition of parse_log_tag_element function.
 */
#pragma once

#include <string_view>
#include <tuple>

#include "num_collect/base/exception.h"
#include "num_collect/logging/impl/is_log_tag_element_char.h"

namespace num_collect::logging::impl {

/*!
 * \brief Parse an element in log tags.
 *
 * \param[in] input Input starting with an element.
 * \return Remaining inputs after an element.
 */
[[nodiscard]] inline auto parse_log_tag_element(std::string_view input)
    -> std::tuple<std::string_view, std::string_view> {
    // NOLINTNEXTLINE(readability-qualified-auto): This is implementation-defined type.
    auto iter = input.begin();
    for (; iter < input.end(); ++iter) {
        if (!is_log_tag_element_char(*iter)) {
            break;
        }
    }
    if (iter == input.begin()) {
        throw base::invalid_argument("Log tag format error.");
    }
    return {std::string_view(input.begin(), iter),
        std::string_view(iter, input.end())};
}

}  // namespace num_collect::logging::impl
