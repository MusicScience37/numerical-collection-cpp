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
 * \brief Profiling of writing trace logs.
 */
#include <chrono>
#include <memory>

#include <gperftools/profiler.h>

#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/sinks/log_sinks.h"

static void test() {
    num_collect::logging::logger logger;
    constexpr std::size_t num_logs = 10000;
    for (std::size_t i = 0; i < num_logs; ++i) {
        logger.trace()("Test trace logs. i={}", i);
    }
}

auto main() -> int {
    num_collect::logging::set_default_tag_config(
        num_collect::logging::log_tag_config()
            .sink(std::make_shared<num_collect::logging::simple_log_sink>(
                "num_collect_prof_write_trace_logs_out.log"))
            .output_log_level(num_collect::logging::log_level::trace));

    constexpr std::size_t repetition = 100;
    ProfilerStart("num_collect_prof_write_trace_logs.prof");
    for (std::size_t i = 0; i < repetition; ++i) {
        test();
    }
    ProfilerStop();
    return 0;
}
