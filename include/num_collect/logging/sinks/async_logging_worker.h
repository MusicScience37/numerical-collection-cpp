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
 * \brief Definition of async_logging_worker class.
 */
#pragma once

#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/util/producer_consumer_circular_queue.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Configurations of the worker of asynchronous logging.
 *
 * \thread_safety Only different objects are usable thread-safely.
 */
class async_logging_worker_config {
public:
    /*!
     * \brief Constructor.
     */
    async_logging_worker_config() = default;

    /*!
     * \brief Get the size of queues for threads.
     *
     * \return Value.
     */
    [[nodiscard]] auto thread_queue_size() const noexcept -> index_type {
        return thread_queue_size_;
    }

    /*!
     * \brief Set the size of queues for threads.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto thread_queue_size(index_type val) -> async_logging_worker_config& {
        if (val <= 0 || val == std::numeric_limits<index_type>::max()) {
            throw invalid_argument(fmt::format("Invalid queue size {}.", val));
        }
        thread_queue_size_ = val;
        return *this;
    }

    /*!
     * \brief Get the maximum number of logs processed at once per thread.
     *
     * \return Value.
     */
    [[nodiscard]] auto max_logs_at_once_per_thread() const noexcept
        -> index_type {
        return max_logs_at_once_per_thread_;
    }

    /*!
     * \brief Set the maximum number of logs processed at once per thread.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_logs_at_once_per_thread(index_type val)
        -> async_logging_worker_config& {
        if (val <= 0) {
            throw invalid_argument(
                fmt::format("Invalid maximum number of logs processed at once "
                            "per thread. {}.",
                    val));
        }
        max_logs_at_once_per_thread_ = val;
        return *this;
    }

    /*!
     * \brief Get the time to wait the next log when no log exists in queues.
     *
     * \return Value.
     */
    [[nodiscard]] auto log_wait_time() const noexcept
        -> std::chrono::microseconds {
        return log_wait_time_;
    }

    /*!
     * \brief Set the time to wait the next log when no log exists in queues.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto log_wait_time(std::chrono::microseconds val)
        -> async_logging_worker_config& {
        if (val <= std::chrono::microseconds(0)) {
            throw invalid_argument(
                fmt::format("Invalid time to wait the next log when no log "
                            "exists in queues. {} us.",
                    val.count()));
        }
        log_wait_time_ = val;
        return *this;
    }

private:
    /*!
     * \brief Default size of queues for threads.
     *
     * \note Queues for threads use one more element in current implementation.
     */
    static constexpr index_type default_thread_queue_size = (1U << 10U) - 1U;

    //! Size of queues for threads.
    index_type thread_queue_size_{default_thread_queue_size};

    //! Default maximum number of logs processed at once per thread.
    static constexpr index_type default_max_logs_at_once_per_thread = 100;

    //! Maximum number of logs processed at once per thread.
    index_type max_logs_at_once_per_thread_{
        default_max_logs_at_once_per_thread};

    //! Default time to wait the next log when no log exists in queues.
    static constexpr std::chrono::microseconds default_log_wait_time{100};

    //! Time to wait the next log when no log exists in queues.
    std::chrono::microseconds log_wait_time_{default_log_wait_time};
};

namespace impl {

/*!
 * \brief Struct of data to request asynchronous processing of logs.
 *
 * \thread_safety Only different objects are usable thread-safely.
 */
struct async_log_request {
public:
    //! Time.
    std::chrono::system_clock::time_point time;

    //! Tag.
    std::string tag;

    //! Log level.
    log_level level;

    //! Filepath.
    std::string file_path;

    //! Line number.
    index_type line;

    //! Column number.
    index_type column;

    //! Function name.
    std::string function_name;

    //! Log body.
    std::string body;

    //! Log sink to write to.
    std::shared_ptr<log_sink_base> sink;
};

//! Type of queues of asynchronous logs for threads.
using async_log_thread_queue_type =
    util::producer_consumer_circular_queue<async_log_request>;

/*!
 * \brief Class of a queue of queues of asynchronous logs for threads.
 *
 * \thread_safety Every operation even for the same object is thread safe.
 */
class async_log_thread_queue_notifier {
public:
    /*!
     * \brief Push a queue.
     *
     * \note This function assumes the input is not null.
     *
     * \param[in] ptr Pointer to the queue.
     */
    void push(std::shared_ptr<async_log_thread_queue_type> ptr) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(std::move(ptr));
    }

    /*!
     * \brief Try to pop a queue.
     *
     * \return Pointer to the queue if exists, otherwise null pointer.
     */
    [[nodiscard]] auto try_pop()
        -> std::shared_ptr<async_log_thread_queue_type> {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) [[likely]] {
            return nullptr;
        }
        auto ptr = std::move(queue_.front());
        queue_.pop();
        return ptr;
    }

    /*!
     * \brief Get the global instance.
     *
     * \return Instance.
     */
    [[nodiscard]] static auto instance() -> async_log_thread_queue_notifier& {
        static async_log_thread_queue_notifier notifier{};
        return notifier;
    }

    /*!
     * \brief Constructor.
     *
     * \warning This constructor is publicly available only for tests of this
     * class.
     */
    async_log_thread_queue_notifier() = default;

    /*!
     * \brief Destructor.
     */
    ~async_log_thread_queue_notifier() = default;

    async_log_thread_queue_notifier(
        const async_log_thread_queue_notifier&) = delete;
    async_log_thread_queue_notifier(async_log_thread_queue_notifier&&) = delete;
    auto operator=(const async_log_thread_queue_notifier&) = delete;
    auto operator=(async_log_thread_queue_notifier&&) = delete;

private:
    //! Queue.
    std::queue<std::shared_ptr<async_log_thread_queue_type>> queue_{};

    //! Mutex of the queue.
    std::mutex mutex_{};
};

/*!
 * \brief Class to initialize queues of asynchronous logs for threads.
 */
class async_log_thread_queue_initializer {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] config Configuration.
     */
    explicit async_log_thread_queue_initializer(
        const async_logging_worker_config& config)
        : queue_(std::make_shared<async_log_thread_queue_type>(
              config.thread_queue_size())) {
        async_log_thread_queue_notifier::instance().push(queue_);
    }

    /*!
     * \brief Access the queue.
     *
     * \return Queue.
     */
    [[nodiscard]] auto queue() noexcept -> async_log_thread_queue_type& {
        return *queue_;
    }

private:
    //! Queue.
    std::shared_ptr<async_log_thread_queue_type> queue_;
};

/*!
 * \brief Class of a queue of asynchronous logs.
 *
 * \thread_safety instance, push functions are thread safe, but others are not.
 */
class async_log_queue {
public:
    /*!
     * \brief Push a request of logging.
     *
     * \param[in] request Request of logging.
     */
    void push(async_log_request&& request) {
        if (!this_thread_queue().try_emplace(std::move(request))) [[unlikely]] {
            throw algorithm_failure("Queue of logs is full.");
        }
    }

    //! Enumeration of the result of spin_once function.
    enum class spin_once_result_type {
        //! No queue exists.
        no_thread_queue,
        //! No log exists.
        no_log,
        //! Some logs processed.
        some_logs_processed
    };

    /*!
     * \brief Process logs once.
     *
     * \tparam Function Type of the function to process requests of logging.
     * \param[in] function Function to process requests of logging.
     * \return Result.
     */
    template <typename Function>
    auto spin_once(Function&& function) -> spin_once_result_type {
        collect_new_queues();
        if (thread_queues_.empty()) {
            return spin_once_result_type::no_thread_queue;
        }

        spin_once_result_type res = spin_once_result_type::no_log;
        std::optional<async_log_request> request;
        for (const auto& queue : thread_queues_) {
            for (index_type i = 0; i < config_.max_logs_at_once_per_thread();
                 ++i) {
                if (queue->try_pop(request)) {
                    function(*request);
                    res = spin_once_result_type::some_logs_processed;
                } else {
                    break;
                }
            }
        }
        return res;
    }

    /*!
     * \brief Get the instance.
     *
     * \note Initialization using the configuration is done only in the first
     * invocation.
     *
     * \param[in] config Configuration.
     * \return Instance.
     */
    [[nodiscard]] static auto instance(
        const async_logging_worker_config& config) -> async_log_queue& {
        static async_log_queue queue{config};
        return queue;
    }

    async_log_queue(const async_log_queue&) = delete;
    async_log_queue(async_log_queue&&) = delete;
    auto operator=(const async_log_queue&) = delete;
    auto operator=(async_log_queue&&) = delete;

private:
    /*!
     * \brief Constructor.
     *
     * \param[in] config Configuration.
     */
    explicit async_log_queue(const async_logging_worker_config& config)
        : config_(config) {}

    /*!
     * \brief Get the queue for this thread.
     *
     * \return Queue.
     */
    [[nodiscard]] auto this_thread_queue() const
        -> async_log_thread_queue_type& {
        static thread_local async_log_thread_queue_initializer initializer{
            config_};
        return initializer.queue();
    }

    /*!
     * \brief Destructor.
     */
    ~async_log_queue() = default;

    /*!
     * \brief Collect newly added queues.
     */
    void collect_new_queues() {
        while (true) {
            auto thread_queue =
                async_log_thread_queue_notifier::instance().try_pop();
            if (thread_queue == nullptr) [[likely]] {
                return;
            }
            thread_queues_.push_back(std::move(thread_queue));
        }
    }

    //! Configuration.
    async_logging_worker_config config_;

    //! Queues for threads.
    std::vector<std::shared_ptr<async_log_thread_queue_type>> thread_queues_{};
};

}  // namespace impl

/*!
 * \brief Class to process logs asynchronously.
 */
class async_logging_worker {
public:
    /*!
     * \brief Write a log asynchronously.
     *
     * \param[in] sink Log sink.
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     */
    void async_write(const std::shared_ptr<log_sink_base>& sink,
        std::chrono::system_clock::time_point time, std::string_view tag,
        log_level level, util::source_info_view source, std::string_view body) {
        queue_.push(impl::async_log_request{.time = time,
            .tag = std::string{tag},
            .level = level,
            .file_path = std::string{source.file_path()},
            .line = source.line(),
            .column = source.column(),
            .function_name = std::string{source.function_name()},
            .body = std::string{body},
            .sink = sink});
    }

    /*!
     * \brief Start this worker.
     *
     * \note This is called from the constructor.
     */
    void start() {
        std::unique_lock<std::mutex> lock(worker_thread_mutex_);
        is_enabled_.store(true, std::memory_order::relaxed);
        if (!worker_thread_.joinable()) {
            worker_thread_ = std::thread{[this] { work(); }};
        }
    }

    /*!
     * \brief Stop this worker.
     *
     * \note This is called from the destructor.
     */
    void stop() {
        std::unique_lock<std::mutex> lock(worker_thread_mutex_);
        worker_thread_end_deadline_ =
            std::chrono::steady_clock::now() + worker_thread_end_timeout;
        is_enabled_.store(false, std::memory_order::release);
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }

    /*!
     * \brief Get the instance.
     *
     * \note Initialization using the configuration is done only in the first
     * invocation.
     *
     * \param[in] config Configuration.
     * \return Instance.
     */
    [[nodiscard]] static auto instance(
        const async_logging_worker_config& config) -> async_logging_worker& {
        static async_logging_worker worker{config};
        return worker;
    }

    /*!
     * \brief Get the instance.
     *
     * \note If this function is called before another overload with a
     * configuration, the queue is initialized with the default configuration.
     *
     * \return Instance.
     */
    [[nodiscard]] static auto instance() -> async_logging_worker& {
        static async_logging_worker_config config{};
        return instance(config);
    }

    async_logging_worker(const async_logging_worker&) = delete;
    async_logging_worker(async_logging_worker&&) = delete;
    auto operator=(const async_logging_worker&) = delete;
    auto operator=(async_logging_worker&&) = delete;

private:
    /*!
     * \brief Constructor.
     *
     * \param[in] config Configuration.
     */
    explicit async_logging_worker(const async_logging_worker_config& config)
        : config_(config), queue_(impl::async_log_queue::instance(config)) {
        start();
    }

    /*!
     * \brief Destructor.
     */
    ~async_logging_worker() { stop(); }

    /*!
     * \brief Process logs. (For worker thread.)
     */
    void work() {
        while (true) {
            try {
                const bool continue_work = work_once();
                if (!continue_work) {
                    return;
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception in worker thread: " << e.what()
                          << std::endl;
            }
        }
    }

    /*!
     * \brief Process logs once.
     *
     * \return Whether to continue.
     */
    [[nodiscard]] auto work_once() -> bool {
        const auto result =
            queue_.spin_once([](const impl::async_log_request& request) {
                request.sink->write(request.time, request.tag, request.level,
                    util::source_info_view{request.file_path, request.line,
                        request.column, request.function_name},
                    request.body);
            });

        const bool is_enabled = is_enabled_.load(std::memory_order::relaxed);
        if (result !=
            impl::async_log_queue::spin_once_result_type::some_logs_processed) {
            if (!is_enabled) {
                return false;
            }
            std::this_thread::sleep_for(config_.log_wait_time());
            return true;
        }

        if (is_enabled) {
            return true;
        }
        std::atomic_thread_fence(std::memory_order::acquire);
        return std::chrono::steady_clock::now() <= *worker_thread_end_deadline_;
    }

    //! Configuration.
    async_logging_worker_config config_;

    //! Queue.
    impl::async_log_queue& queue_;

    //! Worker thread.
    std::thread worker_thread_{};

    //! Whether this worker is enabled.
    std::atomic<bool> is_enabled_{true};

    //! Mutex of thread.
    std::mutex worker_thread_mutex_{};

    //! Deadline of the end of the worker thread.
    std::optional<std::chrono::steady_clock::time_point>
        worker_thread_end_deadline_{};

    //! Timeout of the end of the worker thread.
    static constexpr auto worker_thread_end_timeout = std::chrono::seconds(1);
};

}  // namespace num_collect::logging::sinks
