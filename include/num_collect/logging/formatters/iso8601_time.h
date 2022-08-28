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
 * \brief Definition of iso8601_time class.
 */
#pragma once

#include <chrono>

#include <fmt/chrono.h>
#include <fmt/format.h>

namespace num_collect::logging::formatters {

/*!
 * \brief Class to hold time for formatting it as in ISO 8601.
 */
class iso8601_time {
public:
    /*!
     * \brief Constructor.
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
     * \tparam OutputIterator Type of the output iterator.
     * \param[in] out Output iterator.
     * \return Output iterator after formatting.
     */
    template <typename OutputIterator>
    [[nodiscard]] auto format_to(OutputIterator out) const -> OutputIterator {
        const auto time_sec =
            std::chrono::time_point_cast<std::chrono::seconds>(time_);
        const auto time_tm =
            fmt::gmtime(std::chrono::system_clock::to_time_t(time_sec));
        const auto time_usec =
            std::chrono::duration_cast<std::chrono::microseconds>(
                time_ - time_sec)
                .count();

        return fmt::format_to(
            out, FMT_STRING("{0:%FT%T}.{1:06d}"), time_tm, time_usec);
    }

private:
    //! Time.
    std::chrono::system_clock::time_point time_;
};

}  // namespace num_collect::logging::formatters

namespace fmt {

/*!
 * \brief fmt::formatter for num_collect::logging::formatters::iso8601_time.
 */
template <>
struct formatter<num_collect::logging::formatters::iso8601_time> {
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
    auto format(num_collect::logging::formatters::iso8601_time val,
        FormatContext& context) {
        return val.format_to(context.out());
    }
};

}  // namespace fmt
