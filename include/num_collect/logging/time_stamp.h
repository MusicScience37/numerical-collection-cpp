/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of time_stamp class.
 */
#pragma once

#include <cstdint>
#include <ctime>

#include <fmt/chrono.h>
#include <fmt/format.h>

#include "num_collect/impl/num_collect_export.h"

namespace num_collect::logging {

/*!
 * \brief Class of time stamps.
 *
 * \note This class uses UTC time.
 */
class NUM_COLLECT_EXPORT time_stamp {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] seconds Number of seconds from the epoch.
     * \param[in] nanoseconds Number of nanoseconds from the time specified by
     * seconds (from 0 to 999999999).
     */
    time_stamp(std::time_t seconds, std::uint32_t nanoseconds) noexcept;

    /*!
     * \brief Get the number of seconds from the epoch.
     *
     * \return Number of seconds from the epoch.
     */
    [[nodiscard]] auto seconds() const noexcept -> std::time_t;

    /*!
     * \brief Get the number of nanoseconds from the time specified by seconds.
     *
     * \return Number of nanoseconds from the time specified by seconds.
     */
    [[nodiscard]] auto nanoseconds() const noexcept -> std::uint32_t;

    /*!
     * \brief Get the current time stamp.
     *
     * \return Time stamp.
     */
    [[nodiscard]] static auto now() noexcept -> time_stamp;

private:
    //! Number of seconds from the epoch.
    std::time_t seconds_;

    //! Number of nanoseconds from the time specified by seconds_ (from 0 to 999999999).
    std::uint32_t nanoseconds_;
};

}  // namespace num_collect::logging

namespace fmt {

/*!
 * \brief fmt::formatter for num_collect::logging::time_stamp.
 */
template <>
struct formatter<num_collect::logging::time_stamp> {
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
        num_collect::logging::time_stamp val, FormatContext& context) const {
        const auto time_tm = fmt::gmtime(val.seconds());
        return fmt::format_to(context.out(), FMT_STRING("{0:%FT%T}.{1:09d}"),
            time_tm, val.nanoseconds());
    }
};

}  // namespace fmt
