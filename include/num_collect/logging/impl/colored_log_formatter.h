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
 * \brief Definition of colored_log_formatter class.
 */
#pragma once

#include <chrono>
#include <iterator>
#include <string_view>

#include <fmt/color.h>
#include <fmt/format.h>

#include "num_collect/logging/impl/log_formatter.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::impl {

/*!
 * \brief Get the style for a log level.
 *
 * \param[in] level Log level.
 * \return Style.
 */
[[nodiscard]] inline auto get_log_level_style(log_level level)
    -> fmt::text_style {
    switch (level) {
    case log_level::trace:
        return fmt::fg(fmt::color::gray);
    case log_level::iteration:
    case log_level::iteration_label:
        return fmt::text_style();
    case log_level::summary:
        return fmt::fg(fmt::color::dark_cyan);
    case log_level::info:
        return fmt::fg(fmt::color::green);
    case log_level::warning:
        return fmt::fg(fmt::color::orange_red);
    case log_level::error:
        return fmt::fg(fmt::color::red) | fmt::emphasis::bold;
    default:
        return fmt::text_style();
    }
}

/*!
 * \brief Get the style for a log body.
 *
 * \param[in] level Log level.
 * \return Style.
 */
[[nodiscard]] inline auto get_body_style(log_level level) -> fmt::text_style {
    switch (level) {
    case log_level::iteration_label:
        return fmt::emphasis::bold | fmt::fg(fmt::color::gray);
    case log_level::error:
        return fmt::emphasis::bold | fmt::fg(fmt::color::red);
    default:
        return fmt::text_style();
    }
}

/*!
 * \brief Class to format logs with colors.
 */
class colored_log_formatter {
public:
    /*!
     * \brief Format a log.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     * \return Formatted log.
     *
     * \warning Returned string will be altered at the next call of this
     * function for the same object.
     */
    [[nodiscard]] auto format(std::chrono::system_clock::time_point time,
        std::string_view tag, log_level level, util::source_info_view source,
        std::string_view body) -> std::string_view {
        buffer_.clear();

        auto out = std::back_inserter(buffer_);
        out = fmt::format_to(out, FMT_STRING("[{}] "), iso8601_time(time));
        out = fmt::format_to(out, get_log_level_style(level),
            FMT_STRING("[{}]"), get_output_log_level_str(level));
        out = fmt::format_to(out, FMT_STRING(" [{}] "), tag);
        out =
            fmt::format_to(out, get_body_style(level), FMT_STRING("{}"), body);
        out = fmt::format_to(out, fmt::fg(fmt::color::gray),
            FMT_STRING(" ({}:{}:{}, {})"), source.file_path(), source.line(),
            source.column(), source.function_name());

        return std::string_view(buffer_.data(), buffer_.size());
    }

private:
    //! Buffer.
    fmt::memory_buffer buffer_{};
};

}  // namespace num_collect::logging::impl
