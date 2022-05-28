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
 * \brief Definition of separate_top_log_tag_element function.
 */
#pragma once

#include <string_view>
#include <utility>

#include "num_collect/base/exception.h"
#include "num_collect/logging/impl//log_tag_element.h"

namespace num_collect::logging::impl {

/*!
 * \brief Separate the top level element in a log tag.
 *
 * \param[in] tag Log tag.
 * \return Top level element and remaining.
 */
[[nodiscard]] inline auto separate_top_log_tag_element(std::string_view tag)
    -> std::pair<log_tag_element, std::string_view> {
    const std::size_t size = tag.size();
    if (tag.empty() || tag[0] == ':') {
        throw base::invalid_argument("Log tag format error.");
    }
    for (std::size_t i = 1; i < size; ++i) {
        if (tag[i] == ':') {
            if (i + 2 < size && tag[i + 1] == ':' && tag[i + 2] != ':') {
                return {log_tag_element(std::string(tag.substr(0, i))),
                    tag.substr(i + 2)};
            }
            throw base::invalid_argument("Log tag format error.");
        }
    }
    return {log_tag_element(std::string(tag)), std::string_view()};
}

}  // namespace num_collect::logging::impl
