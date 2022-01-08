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

#include <string_view>

#include <fmt/format.h>

namespace num_collect::logging {

/*!
 * \brief Enumeration of log levels.
 */
enum class log_level {
    //! For internal trace logs.
    trace,

    //! For logs of iterations.
    iteration,

    //! For labels of iteration logs.
    iteration_label,

    //! For summary of calculations.
    summary,

    //! For some information. (Meant for use in user code, not in algorithms.)
    info,

    //! For warnings.
    warning,

    //! For errors.
    error
};

/*!
 * \brief Get the log level string.
 *
 * \param[in] level Log level.
 * \return String.
 */
[[nodiscard]] inline auto get_log_level_str(log_level level) noexcept
    -> std::string_view {
    switch (level) {
    case log_level::trace:
        return "trace";
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
     * \tparam FormatContext Type of the context.
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    template <typename FormatContext>
    auto format(num_collect::logging::log_level val, FormatContext& context) {
        return formatter<string_view>::format(
            num_collect::logging::get_log_level_str(val), context);
    }
};

}  // namespace fmt
