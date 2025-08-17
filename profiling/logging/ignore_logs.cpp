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
 * \brief Profiling of ignore logs.
 */
#include <gperftools/profiler.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config_node.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/sinks/file_log_sink.h"

static void test() {
    num_collect::logging::logger logger;
    constexpr std::size_t num_logs = 1000000;
    for (std::size_t i = 0; i < num_logs; ++i) {
        NUM_COLLECT_LOG_TRACE(logger, "Test trace logs. i={}", i);
    }
}

auto main() -> int {
    num_collect::logging::edit_default_log_tag_config()
        .sink(num_collect::logging::sinks::create_single_file_sink(
            "profile_logging_ignore_logs_out.log"))
        .output_log_level(num_collect::logging::log_level::info);

    constexpr std::size_t repetition = 100;
    ProfilerStart("profile_logging_ignore_logs.prof");
    for (std::size_t i = 0; i < repetition; ++i) {
        test();
    }
    ProfilerStop();
    return 0;
}
