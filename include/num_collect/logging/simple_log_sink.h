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
 * \brief Definition of simple_log_sink class.
 */
#pragma once

#include <chrono>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/impl/log_formatter.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_sink_base.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging {

/*!
 * \brief Class of a simple implementation of log sinks.
 */
class simple_log_sink final : public log_sink_base {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] file File pointer.
     * \param[in] close_on_destruction Whether to close file on destruction.
     */
    simple_log_sink(std::FILE* file, bool close_on_destruction)
        : file_(file), close_on_destruction_(close_on_destruction) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] file_path File path to write logs.
     */
    explicit simple_log_sink(const std::string& file_path)
        : simple_log_sink(open_file(file_path), true) {}

    simple_log_sink(const simple_log_sink&) = delete;
    simple_log_sink(simple_log_sink&&) = delete;
    auto operator=(const simple_log_sink&) -> simple_log_sink& = delete;
    auto operator=(simple_log_sink&&) -> simple_log_sink& = delete;

    /*!
     * \brief Destructor.
     */
    ~simple_log_sink() noexcept override {
        if (file_ != nullptr && close_on_destruction_) {
            (void)std::fclose(file_);
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
     */
    void write(std::chrono::system_clock::time_point time, std::string_view tag,
        log_level level, util::source_info_view source,
        std::string_view body) noexcept override {
        try {
            std::unique_lock<std::mutex> lock(mutex_);
            const auto formatted =
                formatter_.format(time, tag, level, source, body);
            (void)std::fwrite(formatted.data(), 1, formatted.size(), file_);
            (void)std::fputc('\n', file_);
            (void)std::fflush(file_);
        } catch (const std::exception& e) {
            std::cerr << "ERROR IN LOGGING: " << e.what() << std::endl;
        }
    }

private:
    /*!
     * \brief Open a file.
     *
     * \param[in] file_path File path.
     * \return File pointer.
     */
    [[nodiscard]] static auto open_file(const std::string& file_path)
        -> std::FILE* {
        const auto dir_path =
            std::filesystem::absolute(std::filesystem::path(file_path))
                .parent_path();
        std::filesystem::create_directories(dir_path);

        std::FILE* file = std::fopen(file_path.data(), "w");
        if (file == nullptr) {
            throw file_error(
                fmt::format(FMT_STRING("Failed to open {}."), file_path));
        }

        return file;
    }

    //! File pointer.
    std::FILE* file_{nullptr};

    //! Whether to close file on destruction.
    bool close_on_destruction_{false};

    //! Mutex.
    std::mutex mutex_{};

    //! Log formatter.
    impl::log_formatter formatter_{};
};

}  // namespace num_collect::logging
