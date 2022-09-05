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
 * \brief Definition of combined_log_sink class.
 */
#pragma once

#include <chrono>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Class of log sinks to write logs to multiple log sinks.
 */
class combined_log_sink final : public log_sink_base {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] sinks Log sinks with log levels.
     */
    explicit combined_log_sink(
        std::vector<std::pair<std::shared_ptr<log_sink_base>, log_level>> sinks)
        : sinks_(std::move(sinks)) {}

    /*!
     * \brief Destructor.
     */
    ~combined_log_sink() override = default;

    combined_log_sink(const combined_log_sink&) = delete;
    combined_log_sink(combined_log_sink&&) = delete;
    auto operator=(const combined_log_sink&) -> combined_log_sink& = delete;
    auto operator=(combined_log_sink&&) -> combined_log_sink& = delete;

    /*!
     * \brief Write a log.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     *
     * \note Implementations of this function must be thread-safe.
     */
    void write(std::chrono::system_clock::time_point time, std::string_view tag,
        log_level level, util::source_info_view source,
        std::string_view body) noexcept override {
        for (const auto& [sink, output_log_level] : sinks_) {
            if (level >= output_log_level) {
                sink->write(time, tag, level, source, body);
            }
        }
    }

    /*!
     * \brief Write a log.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     *
     * \note Implementations of this function must be thread-safe.
     */
    void write(std::chrono::system_clock::time_point time, std::string_view tag,
        log_level level, util::source_info_view source,
        fmt::memory_buffer&& body) noexcept override {
        for (const auto& [sink, output_log_level] : sinks_) {
            if (level >= output_log_level) {
                sink->write(time, tag, level, source, std::move(body));
            }
        }
    }

private:
    //! Log sinks with log levels.
    std::vector<std::pair<std::shared_ptr<log_sink_base>, log_level>> sinks_;
};

}  // namespace num_collect::logging::sinks
