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
 * \brief Definition of colored_compact_log_formatter class.
 */
#pragma once

#include <cstddef>
#include <iterator>
#include <string_view>

#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/format.h>

#include "num_collect/logging/formatters/get_log_level_str_for_log.h"
#include "num_collect/logging/formatters/log_formatter_base.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::formatters {

namespace impl {

/*!
 * \brief Get the style for a log level.
 *
 * \param[in] level Log level.
 * \return Style.
 */
[[nodiscard]] inline auto get_log_level_style(
    log_level level) -> fmt::text_style {
    switch (level) {
    case log_level::trace:
        return fmt::fg(fmt::color::gray);
    case log_level::debug:
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
    case log_level::critical:
        return fmt::fg(fmt::color::red) | fmt::bg(fmt::color::yellow) |
            fmt::emphasis::bold;
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
    case log_level::critical:
        return fmt::emphasis::bold | fmt::fg(fmt::color::red) |
            fmt::bg(fmt::color::yellow);
    default:
        return fmt::text_style();
    }
}

}  // namespace impl

/*!
 * \brief Class to format logs compactly with colors.
 *
 * \note This formatter is meant for logs written to files.
 *
 * \thread_safety Thread-safe for all operations.
 */
class colored_compact_log_formatter : public log_formatter_base {
public:
    //! \copydoc num_collect::logging::formatters::log_formatter_base::format
    void format(fmt::memory_buffer& buffer, time_stamp time,
        std::string_view tag, log_level level, util::source_info_view source,
        std::string_view body) override {
        std::string_view filename = source.file_path();
        const std::size_t last_separator_pos = filename.find_last_of("/\\");
        if (last_separator_pos != std::string_view::npos) {
            filename = filename.substr(last_separator_pos + 1);
        }

        auto out = std::back_inserter(buffer);
        out = fmt::format_to(out, FMT_STRING("[{}] "), time);
        out = fmt::format_to(out, impl::get_log_level_style(level),
            FMT_STRING("[{}]"), get_output_log_level_str(level));
        out = fmt::format_to(out, FMT_STRING(" [{}] "), tag);
        out = fmt::format_to(
            out, impl::get_body_style(level), FMT_STRING("{}"), body);
        out = fmt::format_to(out, fmt::fg(fmt::color::gray),
            FMT_STRING(" ({}:{}, {})"), filename, source.line(),
            source.function_name());
    }

    //! Constructor.
    colored_compact_log_formatter() = default;

    //! Destructor.
    ~colored_compact_log_formatter() override = default;

    colored_compact_log_formatter(
        const colored_compact_log_formatter&) = delete;
    colored_compact_log_formatter(colored_compact_log_formatter&&) = delete;
    auto operator=(const colored_compact_log_formatter&) = delete;
    auto operator=(colored_compact_log_formatter&&) = delete;
};

}  // namespace num_collect::logging::formatters
