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
 * \brief Definition of compact_log_formatter class.
 */
#pragma once

#include <cstddef>
#include <iterator>
#include <string_view>

#include <fmt/format.h>

#include "num_collect/logging/formatters/get_log_level_str_for_log.h"
#include "num_collect/logging/formatters/log_formatter_base.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::formatters {

/*!
 * \brief Class to format logs compactly.
 *
 * \note This formatter is meant for logs written to files.
 *
 * \thread_safety Thread-safe for all operations.
 */
class compact_log_formatter : public log_formatter_base {
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

        fmt::format_to(std::back_inserter(buffer),
            FMT_STRING("[{}] [{}] [{}] {} ({}:{}, {})"), time,
            get_output_log_level_str(level), tag, body, filename, source.line(),
            source.function_name());
    }

    //! Constructor.
    compact_log_formatter() = default;

    //! Destructor.
    ~compact_log_formatter() override = default;

    compact_log_formatter(const compact_log_formatter&) = delete;
    compact_log_formatter(compact_log_formatter&&) = delete;
    auto operator=(const compact_log_formatter&) = delete;
    auto operator=(compact_log_formatter&&) = delete;
};

}  // namespace num_collect::logging::formatters
