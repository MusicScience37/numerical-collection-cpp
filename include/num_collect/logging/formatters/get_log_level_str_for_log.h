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
 * \brief Definition of get_output_log_level_str function.
 */
#pragma once

#include <string_view>

#include "num_collect/logging/log_level.h"

namespace num_collect::logging::formatters {

/*!
 * \brief Get the log level string for outputs.
 *
 * \param[in] level Log level.
 * \return String.
 */
[[nodiscard]] inline auto get_output_log_level_str(
    log_level level) noexcept -> std::string_view {
    switch (level) {
    case log_level::trace:
        return "trace";
    case log_level::debug:
        return "debug";
    case log_level::iteration:
    case log_level::iteration_label:
        return "iteration";
    case log_level::summary:
        return "summary";
    case log_level::info:
        return "info";
    case log_level::warning:
        return "warning";
    case log_level::error:
        return "error";
    case log_level::critical:
        return "critical";
    default:
        return "unknown";
    }
}

}  // namespace num_collect::logging::formatters
