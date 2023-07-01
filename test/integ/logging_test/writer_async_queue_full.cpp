/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Test to write logs.
 */
#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>

#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/sinks/async_logging_worker.h"
#include "num_collect/logging/sinks/async_logging_worker_config.h"

auto main(int argc, char** argv) -> int {
    try {
        if (argc != 2) {
            std::cerr
                << "Usage: "
                << argv[0]  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                << " <configuration-file>" << std::endl;
            return 1;
        }

        num_collect::logging::sinks::init_async_logging_worker(
            num_collect::logging::sinks::async_logging_worker_config()
                .thread_queue_size(10));  // NOLINT
        num_collect::logging::sinks::stop_async_logging_worker();

        const std::string_view config_filepath =
            argv[1];  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        num_collect::logging::load_logging_config_file(
            std::string{config_filepath});

        num_collect::logging::logger logger{};
        for (std::size_t i = 0; i < 100; ++i) {  // NOLINT
            logger.info()("test");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // NOLINT

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception thrown: " << e.what() << std::endl;
        return 1;
    }
}
