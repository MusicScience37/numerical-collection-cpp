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
 * \brief Test of async_logging_worker class.
 */
#include "num_collect/logging/sinks/async_logging_worker.h"

#include <chrono>
#include <limits>
#include <memory>
#include <thread>

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
}

TEST_CASE(
    "num_collect::logging::sinks::impl::async_log_thread_queue_notifier") {
    using num_collect::logging::sinks::impl::async_log_thread_queue_notifier;
    using num_collect::logging::sinks::impl::async_log_thread_queue_type;

    SECTION("check thread safety") {
        static constexpr std::size_t num_elems = 100;
        static constexpr auto timeout = std::chrono::seconds(10);
        static constexpr auto wait_time = std::chrono::microseconds(100);

        async_log_thread_queue_notifier notifier;

        std::vector<async_log_thread_queue_type*> output;
        output.reserve(num_elems);
        std::thread consumer{[&notifier, &output] {
            const auto deadline = std::chrono::steady_clock::now() + timeout;
            while (output.size() < num_elems &&
                std::chrono::steady_clock::now() < deadline) {
                async_log_thread_queue_type* ptr = notifier.try_pop();
                if (ptr == nullptr) {
                    std::this_thread::sleep_for(wait_time);
                } else {
                    output.push_back(ptr);
                }
            }
        }};

        std::vector<std::unique_ptr<async_log_thread_queue_type>> input;
        input.reserve(num_elems);
        for (std::size_t i = 0; i < num_elems; ++i) {
            input.push_back(std::make_unique<async_log_thread_queue_type>(1));
        }
        for (const auto& elem : input) {
            notifier.push(elem.get());
        }

        consumer.join();

        for (std::size_t i = 0; i < num_elems; ++i) {
            INFO("i = " << i);
            CHECK(static_cast<void*>(output.at(i)) ==
                static_cast<void*>(input.at(i).get()));
        }
    }
}
