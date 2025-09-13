/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of the function to create asynchronous log sinks.
 */
#include "num_collect/logging/sinks/async_log_sink.h"

#include <array>
#include <cstring>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>
#include <type_traits>
#include <variant>

#include <moodycamel/blockingconcurrentqueue.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Class of logs written asynchronously.
 */
class async_log {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     */
    async_log(time_stamp time, std::string_view tag, log_level level,
        util::source_info_view source, std::string_view body)
        : time_(time), tag_(tag), level_(level), source_(source), body_(body) {
        const std::size_t total_size = tag.size() + source.file_path().size() +
            source.function_name().size() + body.size();
        // NOLINTNEXTLINE(*-c-arrays): false positive
        buffer_ = std::make_unique_for_overwrite<char[]>(total_size);

        char* begin = buffer_.get();
        std::memcpy(begin, tag.data(), tag.size());
        tag_ = std::string_view(begin, tag.size());
        begin += tag.size();

        std::memcpy(
            begin, source.file_path().data(), source.file_path().size());
        const std::string_view source_file_path(
            begin, source.file_path().size());
        begin += source.file_path().size();
        std::memcpy(begin, source.function_name().data(),
            source.function_name().size());
        source_ = util::source_info_view(source_file_path, source.line(),
            source.column(),
            std::string_view(begin, source.function_name().size()));
        begin += source.function_name().size();

        std::memcpy(begin, body.data(), body.size());
        body_ = std::string_view(begin, body.size());
    }

    /*!
     * \brief Get the time.
     *
     * \return Time.
     */
    [[nodiscard]] auto time() const noexcept -> time_stamp { return time_; }

    /*!
     * \brief Get the tag.
     *
     * \return Tag.
     */
    [[nodiscard]] auto tag() const noexcept -> std::string_view { return tag_; }

    /*!
     * \brief Get the log level.
     *
     * \return Log level.
     */
    [[nodiscard]] auto level() const noexcept -> log_level { return level_; }

    /*!
     * \brief Get the source information.
     *
     * \return Source information.
     */
    [[nodiscard]] auto source() const noexcept -> util::source_info_view {
        return source_;
    }

    /*!
     * \brief Get the log body.
     *
     * \return Log body.
     */
    [[nodiscard]] auto body() const noexcept -> std::string_view {
        return body_;
    }

private:
    //! Time.
    time_stamp time_;

    //! Tag.
    std::string_view tag_;

    //! Log level.
    log_level level_;

    //! Source information.
    util::source_info_view source_;

    //! Log body.
    std::string_view body_;

    //! Buffer of strings.
    std::unique_ptr<char[]> buffer_;  // NOLINT(*-c-arrays): false positive
};

/*!
 * \brief Class of log sinks to write logs asynchronously.
 */
class async_log_sink {
public:
    //! Tag to specify shutdown.
    struct shutdown_tag {};

    //! Type of items in the queue.
    using item_type = std::variant<shutdown_tag, async_log>;

    /*!
     * \brief Constructor.
     *
     * \param[in] sink Log sink to write logs.
     */
    explicit async_log_sink(log_sink sink) : sink_(std::move(sink)) {
        thread_ = std::thread(&async_log_sink::write_logs, this);
    }

    async_log_sink(const async_log_sink&) = delete;
    async_log_sink(async_log_sink&&) = delete;
    auto operator=(const async_log_sink&) -> async_log_sink& = delete;
    auto operator=(async_log_sink&&) -> async_log_sink& = delete;

    /*!
     * \brief Destructor.
     *
     * \note This function will write all logs remaining in the queue before
     * destruction.
     */
    ~async_log_sink() {
        queue_.enqueue(shutdown_tag{});
        thread_.join();
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
     * \note This function can be called from multiple threads.
     */
    void write(time_stamp time, std::string_view tag, log_level level,
        util::source_info_view source, std::string_view body) noexcept {
        try {
            item_type item{async_log{time, tag, level, source, body}};
            queue_.enqueue(std::move(item));
        } catch (const std::exception& e) {
            // Because this function cannot throw exceptions,
            // just print the error message.
            std::cerr << "ERROR IN LOGGING: " << e.what()
                      << std::endl;  // NOLINT(*-avoid-endl): intentional
        }
    }

private:
    /*!
     * \brief Write logs. (Entry point of the thread.)
     */
    void write_logs() noexcept {
        // TODO tune buffer size.
        constexpr std::size_t buffer_size = 16;
        std::array<item_type, buffer_size> buffer{};
        bool is_enabled = true;
        while (is_enabled) {
            std::size_t num_dequeued =
                queue_.try_dequeue_bulk(buffer.data(), buffer.size());
            if (num_dequeued == 0) {
                if (!is_enabled) {
                    return;
                }
                num_dequeued =
                    queue_.wait_dequeue_bulk(buffer.data(), buffer.size());
            }
            NUM_COLLECT_ASSERT(num_dequeued > 0);

            for (std::size_t i = 0; i < num_dequeued; ++i) {
                const item_type& item = buffer[i];
                std::visit(
                    [this, &is_enabled](const auto& actual_item) {
                        using actual_item_type =
                            std::decay_t<decltype(actual_item)>;
                        if constexpr (std::is_same_v<actual_item_type,
                                          async_log>) {
                            sink_.write(actual_item.time(), actual_item.tag(),
                                actual_item.level(), actual_item.source(),
                                actual_item.body());
                        } else {
                            // shutdown_tag
                            static_assert(
                                std::is_same_v<actual_item_type, shutdown_tag>,
                                "Unexpected type of item.");
                            is_enabled = false;
                        }
                    },
                    item);
            }
        }
    }

    //! Type of the queue.
    using queue_type = moodycamel::BlockingConcurrentQueue<item_type>;

    //! Queue.
    queue_type queue_;

    //! Log sink to write logs.
    log_sink sink_;

    //! Thread to write logs.
    std::thread thread_;
};

auto create_async_log_sink(log_sink sink) -> log_sink {
    return create_log_sink<async_log_sink>(std::move(sink));
}

}  // namespace num_collect::logging::sinks
