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
 * \brief Definition of log_level enumeration.
 */
#pragma once

#include <cstdint>
#include <string_view>

#include <fmt/base.h>
#include <fmt/format.h>  // IWYU pragma: keep

namespace num_collect::logging {

// clang-format off
/*!
 * \brief Enumeration of log levels.
 *
 * | Log level       | Usage in this library                                                   |
 * | :-------------- | :---------------------------------------------------------------------- |
 * | trace           | Internal trace logs for developer of this library.                      |
 * | debug           | Log for debugging.                                                      |
 * | iteration       | Log of each iteration.                                                  |
 * | iteration_label | Label of iteration logs. (Not usable for output log level.)             |
 * | summary         | Summary of calculations.                                                |
 * | info            | None. (Meant for use in user code to show some information.)            |
 * | warning         | Warnings of undesirable conditions.                                     |
 * | error           | Errors preventing further processing. (Usually used with exceptions.)   |
 * | critical        | None. (Meant for use in conditions which may cause death of processes.) |
 * | off             | Turn off output. (Only for output log level.)                           |
 */
enum class log_level : std::uint8_t {  // clang-format on
    //! Trace.
    trace,

    //! Debug.
    debug,

    //! Iteration.
    iteration,

    //! Label of iterations.
    iteration_label,

    //! Summary.
    summary,

    //! Information.
    info,

    //! Warning.
    warning,

    //! Error.
    error,

    //! Critical.
    critical,

    //! Turn off output. (Only for output log level).
    off
};

/*!
 * \brief Get the log level string.
 *
 * \param[in] level Log level.
 * \return String.
 */
[[nodiscard]] inline auto get_log_level_str(
    log_level level) noexcept -> std::string_view {
    switch (level) {
    case log_level::trace:
        return "trace";
    case log_level::debug:
        return "debug";
    case log_level::iteration:
        return "iteration";
    case log_level::iteration_label:
        return "iteration_label";
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
    case log_level::off:
        return "off";
    }
    return "unknown";
}

}  // namespace num_collect::logging

namespace fmt {

/*!
 * \brief fmt::formatter for num_collect::logging::log_level.
 */
template <>
struct formatter<num_collect::logging::log_level>
    : public formatter<string_view> {
public:
    /*!
     * \brief Format a value.
     *
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    auto format(
        num_collect::logging::log_level val, format_context& context) const {
        return formatter<string_view>::format(
            num_collect::logging::get_log_level_str(val), context);
    }
};

}  // namespace fmt
