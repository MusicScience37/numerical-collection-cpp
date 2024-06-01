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
 * \brief Definition of parse_log_tag_separator function.
 */
#pragma once

#include <string_view>

#include "num_collect/base/exception.h"

namespace num_collect::logging::impl {

/*!
 * \brief Parse a separator in log tags.
 *
 * \param[in] input Input starting with a separator.
 * \return Remaining inputs after a separator.
 */
[[nodiscard]] inline auto parse_log_tag_separator(
    std::string_view input) -> std::string_view {
    if (input.empty()) {
        throw base::invalid_argument("Log tag format error.");
    }
    constexpr auto colon = static_cast<char>(0x3A);  // :
    if (input[0] == colon) {
        if (input.size() == 1 || input[1] != colon) {
            throw base::invalid_argument("Log tag format error.");
        }
        return input.substr(2);
    }
    constexpr auto period = static_cast<char>(0x2E);
    if (input[0] == period) {
        return input.substr(1);
    }
    throw base::invalid_argument("Log tag format error.");
}

}  // namespace num_collect::logging::impl
