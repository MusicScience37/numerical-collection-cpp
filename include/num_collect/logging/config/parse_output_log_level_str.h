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
 * \brief Definition of parse_output_log_level_str function.
 */
#pragma once

#include <string>
#include <string_view>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/log_level.h"

namespace num_collect::logging::config {

/*!
 * \brief Parse a log level from a string to use in output_log_level of
 * log_tag_config class.
 *
 * \param[in] str String.
 * \return Log level.
 */
[[nodiscard]] inline auto parse_output_log_level_str(std::string_view str)
    -> log_level {
    if (str == "trace") {
        return log_level::trace;
    }
    if (str == "debug") {
        return log_level::debug;
    }
    if (str == "iteration") {
        return log_level::iteration;
    }
    if (str == "summary") {
        return log_level::summary;
    }
    if (str == "info") {
        return log_level::info;
    }
    if (str == "warning") {
        return log_level::warning;
    }
    if (str == "error") {
        return log_level::error;
    }
    if (str == "critical") {
        return log_level::critical;
    }
    if (str == "off") {
        return log_level::off;
    }
    throw invalid_argument(fmt::format("Invalid log level {}.", str));
}

}  // namespace num_collect::logging::config
