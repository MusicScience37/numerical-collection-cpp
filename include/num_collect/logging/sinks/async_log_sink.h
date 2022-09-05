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
 * \brief Definition of async_log_sink class.
 */
#pragma once

#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/async_logging_worker.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Class of log sinks to write logs asynchronously.
 */
class async_log_sink final : public log_sink_base {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] sink Log sink to write logs actually.
     */
    explicit async_log_sink(std::shared_ptr<log_sink_base> sink)
        : sink_(std::move(sink)), worker_(async_logging_worker::instance()) {}

    /*!
     * \brief Destructor.
     */
    ~async_log_sink() override = default;

    async_log_sink(const async_log_sink&) = delete;
    async_log_sink(async_log_sink&&) = delete;
    auto operator=(const async_log_sink&) -> async_log_sink& = delete;
    auto operator=(async_log_sink&&) -> async_log_sink& = delete;

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
        if (!is_enabled_) [[unlikely]] {
            return;
        }
        try {
            worker_.async_write(sink_, time, tag, level, source, body);
        } catch (const std::exception& e) {
            std::cerr << "ERROR IN LOGGING: " << e.what() << std::endl;
            is_enabled_ = false;
        }
    }

private:
    //! Log sink to write logs actually.
    std::shared_ptr<log_sink_base> sink_;

    //! Worker.
    async_logging_worker& worker_;

    //! Whether this object is enabled.
    bool is_enabled_{true};
};

}  // namespace num_collect::logging::sinks
