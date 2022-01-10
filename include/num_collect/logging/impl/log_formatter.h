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
 * \brief Definition of log_formatter class.
 */
#pragma once

#include <chrono>
#include <iterator>
#include <string_view>

#include <fmt/chrono.h>
#include <fmt/format.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::impl {

/*!
 * \brief Class to hold time for formatting it as in ISO 8601.
 *
 */
class iso8601_time {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] time Time.
     */
    explicit iso8601_time(std::chrono::system_clock::time_point time)
        : time_(time) {}

    /*!
     * \brief Get the time.
     *
     * \return Time.
     */
    [[nodiscard]] auto time() const noexcept { return time_; }

    /*!
     * \brief Format time.
     *
     * \tparam Outputiterator Type of the output iterator.
     * \param[in] out Output iterator.
     * \return Output iterator after formatting.
     */
    template <std::output_iterator<char> Outputiterator>
    [[nodiscard]] auto format_to(Outputiterator out) const -> Outputiterator {
        const auto time_sec =
            std::chrono::time_point_cast<std::chrono::seconds>(time_);
        const auto time_tm =
            fmt::localtime(std::chrono::system_clock::to_time_t(time_sec));
        const auto time_usec =
            std::chrono::duration_cast<std::chrono::microseconds>(
                time_ - time_sec)
                .count();

        return fmt::format_to(
            out, FMT_STRING("{0:%FT%T}.{1:06d}{0:%z}"), time_tm, time_usec);
    }

private:
    //! Time.
    std::chrono::system_clock::time_point time_;
};

}  // namespace num_collect::logging::impl

namespace fmt {

/*!
 * \brief fmt::formatter for num_collect::logging::impl::iso8601_time.
 */
template <>
struct formatter<num_collect::logging::impl::iso8601_time> {
public:
    /*!
     * \brief Parse format specifications.
     *
     * \param[in] context Context.
     * \return Iterator.
     */
    constexpr auto parse(format_parse_context& context)  // NOLINT
        -> decltype(context.begin()) {
        return context.end();
    }

    /*!
     * \brief Format a value.
     *
     * \tparam FormatContext Type of the context.
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    template <typename FormatContext>
    auto format(
        num_collect::logging::impl::iso8601_time val, FormatContext& context) {
        return val.format_to(context.out());
    }
};

}  // namespace fmt

namespace num_collect::logging::impl {

/*!
 * \brief Get the log level string for outputs.
 *
 * \param[in] level Log level.
 * \return String.
 */
[[nodiscard]] inline auto get_output_log_level_str(log_level level) noexcept
    -> std::string_view {
    switch (level) {
    case log_level::trace:
        return "trace";
    case log_level::iteration:
    case log_level::iteration_label:
        return "iter";
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

/*!
 * \brief Class to format logs.
 */
class log_formatter {
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
        std::string_view tag, log_level level, source_info_view source,
        std::string_view body) -> std::string_view {
        buffer_.clear();

        fmt::format_to(std::back_inserter(buffer_),
            FMT_STRING("[{}] [{}] [{}] {} ({}:{}:{}, {})"), iso8601_time(time),
            get_output_log_level_str(level), tag, body, source.file_path(),
            source.line(), source.column(), source.function_name());

        return std::string_view(buffer_.data(), buffer_.size());
    }

private:
    //! Buffer.
    fmt::memory_buffer buffer_{};
};

}  // namespace num_collect::logging::impl
