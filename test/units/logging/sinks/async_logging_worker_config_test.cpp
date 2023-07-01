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
 * \brief Test of async_logging_worker_config class.
 */
#include "num_collect/logging/sinks/async_logging_worker_config.h"

#include <chrono>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::logging::sinks::async_logging_worker_config") {
    using num_collect::logging::sinks::async_logging_worker_config;

    SECTION("thread_queue_size") {
        async_logging_worker_config config;
        CHECK(config.thread_queue_size() > 0);

        constexpr num_collect::index_type val = 12345;
        CHECK(config.thread_queue_size(val).thread_queue_size() == val);

        CHECK_THROWS(config.thread_queue_size(-1));
        CHECK_THROWS(config.thread_queue_size(0));
        CHECK_NOTHROW(config.thread_queue_size(1));
        CHECK_NOTHROW(config.thread_queue_size(2));
        CHECK_THROWS(config.thread_queue_size(
            std::numeric_limits<num_collect::index_type>::max()));
    }

    SECTION("max_logs_at_once_per_thread") {
        async_logging_worker_config config;
        CHECK(config.max_logs_at_once_per_thread() > 0);

        constexpr num_collect::index_type val = 123;
        CHECK(config.max_logs_at_once_per_thread(val)
                  .max_logs_at_once_per_thread() == val);

        CHECK_THROWS(config.max_logs_at_once_per_thread(-1));
        CHECK_THROWS(config.max_logs_at_once_per_thread(0));
        CHECK_NOTHROW(config.max_logs_at_once_per_thread(1));
        CHECK_NOTHROW(config.max_logs_at_once_per_thread(2));
    }

    SECTION("log_wait_time") {
        async_logging_worker_config config;
        CHECK(config.log_wait_time().count() > 0);

        constexpr int count = 12345;
        CHECK(config.log_wait_time(std::chrono::microseconds(count))
                  .log_wait_time()
                  .count() == count);

        CHECK_THROWS(config.log_wait_time(std::chrono::microseconds(-1)));
        CHECK_THROWS(config.log_wait_time(std::chrono::microseconds(0)));
        CHECK_NOTHROW(config.log_wait_time(std::chrono::microseconds(1)));
        CHECK_NOTHROW(config.log_wait_time(std::chrono::microseconds(2)));
    }
}
