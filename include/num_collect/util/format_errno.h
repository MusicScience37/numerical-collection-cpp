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
 * \brief Definition of format_errno function.
 */
#pragma once

#include <cerrno>
#include <iterator>
#include <system_error>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

namespace num_collect::util {

/*!
 * \brief Format a message with error message determined by errno.
 *
 * \tparam Args Types of arguments.
 * \param[in] format Format.
 * \param[in] args Arguments.
 * \return Formatted message.
 */
template <typename... Args>
[[nodiscard]] inline auto format_errno(
    fmt::format_string<Args...> format, Args&&... args) -> std::string {
    const int current_errno = errno;
    fmt::memory_buffer buffer;
    fmt::format_to(
        std::back_inserter(buffer), format, std::forward<Args>(args)...);
    if (current_errno != 0) {
        const auto error_code =
            std::error_code(current_errno, std::generic_category());
        fmt::format_to(
            std::back_inserter(buffer), ": {}", error_code.message());
    }
    return std::string(buffer.data(), buffer.size());
}

}  // namespace num_collect::util
