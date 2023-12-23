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
 * \brief Definition of functions for simple log sinks.
 */
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>

#include "num_collect/logging/formatters/colored_compact_log_formatter.h"
#include "num_collect/logging/formatters/compact_log_formatter.h"
#include "num_collect/logging/formatters/detailed_log_formatter.h"
#include "num_collect/logging/formatters/log_formatter_base.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/console_log_sink.h"
#include "num_collect/logging/sinks/file_log_sink.h"
#include "num_collect/logging/sinks/file_wrapper.h"
#include "num_collect/logging/sinks/log_sink.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Class of simple log sink.
 *
 * \thread_safety Thread-safe for all operations.
 */
class simple_log_sink {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] file File.
     * \param[in] formatter Formatter.
     */
    simple_log_sink(file_wrapper file,
        std::shared_ptr<formatters::log_formatter_base> formatter)
        : file_(std::move(file)), formatter_(std::move(formatter)) {}

    simple_log_sink(const simple_log_sink&) = delete;
    simple_log_sink(simple_log_sink&&) = delete;
    auto operator=(const simple_log_sink&) -> simple_log_sink& = delete;
    auto operator=(simple_log_sink&&) -> simple_log_sink& = delete;

    /*!
     * \brief Write a log.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     *
     * \note This function can be called from multiple threads.
     */
    void write(time_stamp time, std::string_view tag, log_level level,
        util::source_info_view source, std::string_view body) noexcept {
        if (!is_enabled_) [[unlikely]] {
            return;
        }
        try {
            std::unique_lock<std::mutex> lock(mutex_);
            fmt::memory_buffer buffer;
            formatter_->format(buffer, time, tag, level, source, body);
            buffer.push_back('\n');
            file_.write(std::string_view(buffer.data(), buffer.size()));
            file_.flush();
        } catch (const std::exception& e) {
            std::cerr << "ERROR IN LOGGING: " << e.what() << std::endl;
            file_.close();
            is_enabled_ = false;
        }
    }

    /*!
     * \brief Destructor.
     */
    ~simple_log_sink() = default;

private:
    //! File.
    file_wrapper file_;

    //! Formatter.
    std::shared_ptr<formatters::log_formatter_base> formatter_;

    //! Mutex.
    std::mutex mutex_{};

    //! Whether this object is enabled.
    bool is_enabled_{true};
};

auto create_single_file_sink(std::string_view filepath) -> log_sink {
    const auto dir_path = std::filesystem::absolute(filepath).parent_path();
    std::filesystem::create_directories(dir_path);
    return create_log_sink<simple_log_sink>(
        file_wrapper{std::string(filepath), "w"},
        std::make_shared<formatters::detailed_log_formatter>());
}

auto create_stdout_file_wrapper() -> file_wrapper {
    file_wrapper file{};
    file.set_stdout();
    return file;
}

auto create_colored_console_sink() -> log_sink {
    static auto sink =
        create_log_sink<simple_log_sink>(create_stdout_file_wrapper(),
            std::make_shared<formatters::colored_compact_log_formatter>());
    return sink;
}

auto create_non_colored_console_sink() -> log_sink {
    static auto sink =
        create_log_sink<simple_log_sink>(create_stdout_file_wrapper(),
            std::make_shared<formatters::compact_log_formatter>());
    return sink;
}

}  // namespace num_collect::logging::sinks
