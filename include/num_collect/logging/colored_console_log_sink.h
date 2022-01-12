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
 * \brief Definition of colored_console_log_sink class.
 */
#pragma once

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <mutex>

#include "num_collect/base/exception.h"
#include "num_collect/logging/impl/colored_log_formatter.h"
#include "num_collect/logging/log_sink_base.h"

namespace num_collect::logging {

/*!
 * \brief Class of an implementation of log sinks with colors in consoles.
 */
class colored_console_log_sink final : public log_sink_base {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] file File pointer.
     *
     * \note This class is meant for console logging, so file pointer will never
     * be closed.
     */
    explicit colored_console_log_sink(std::FILE* file) : file_(file) {}

    colored_console_log_sink(const colored_console_log_sink&) = delete;
    colored_console_log_sink(colored_console_log_sink&&) = delete;
    auto operator=(const colored_console_log_sink&)
        -> colored_console_log_sink& = delete;
    auto operator=(colored_console_log_sink&&)
        -> colored_console_log_sink& = delete;

    /*!
     * \brief Destruct.
     */
    ~colored_console_log_sink() noexcept override = default;

    /*!
     * \brief Write a log.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     */
    void write(std::chrono::system_clock::time_point time, std::string_view tag,
        log_level level, util::source_info_view source,
        std::string_view body) noexcept override {
        try {
            std::unique_lock<std::mutex> lock(mutex_);
            const auto formatted =
                formatter_.format(time, tag, level, source, body);
            std::fwrite(formatted.data(), 1, formatted.size(), file_);
            std::fputc('\n', file_);
            std::fflush(file_);
        } catch (const std::exception& e) {
            std::cerr << "ERROR IN LOGGING: " << e.what() << std::endl;
        }
    }

private:
    //! File pointer.
    std::FILE* file_{nullptr};

    //! Mutex.
    std::mutex mutex_{};

    //! Log formatter.
    impl::colored_log_formatter formatter_{};
};

}  // namespace num_collect::logging
