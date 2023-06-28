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
 * \brief Definition of async_logging_worker_config class.
 */
#pragma once

#include <chrono>
#include <limits>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"

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

}  // namespace num_collect::logging::sinks
