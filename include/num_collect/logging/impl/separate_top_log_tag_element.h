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

#include <string>
#include <string_view>
#include <utility>

#include "num_collect/base/exception.h"
#include "num_collect/logging/impl/log_tag_element.h"
#include "num_collect/logging/impl/parse_log_tag_element.h"
#include "num_collect/logging/impl/parse_log_tag_separator.h"

namespace num_collect::logging::impl {

/*!
 * \brief Separate the top level element in a log tag.
 *
 * \param[in] tag Log tag.
 * \return Top level element and remaining.
 */
[[nodiscard]] inline auto separate_top_log_tag_element(std::string_view tag)
    -> std::pair<log_tag_element, std::string_view> {
    const auto [element, remaining_after_elements] = parse_log_tag_element(tag);
    if (remaining_after_elements.empty()) {
        return {log_tag_element{std::string{element}}, std::string_view{}};
    }

    const auto remaining_elements =
        parse_log_tag_separator(remaining_after_elements);
    if (remaining_elements.empty()) {
        throw base::invalid_argument("Log tag format error.");
    }

    return {log_tag_element{std::string{element}}, remaining_elements};
}

}  // namespace num_collect::logging::impl
