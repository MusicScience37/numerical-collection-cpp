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
 * \brief Benchmark of writing trace logs.
 */
#include <fmt/format.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>

#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/sinks/simple_log_sink.h"

STAT_BENCH_MAIN

static void perform() {
    num_collect::logging::logger logger;
    STAT_BENCH_MEASURE_INDEXED(/*thread_ind*/, /*sample_ind*/, i) {
        logger.trace()("Test trace logs. i={}", i);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("trace_logs", "write no log") {
    num_collect::logging::log_config::instance().set_default_tag_config(
        num_collect::logging::log_tag_config()
            .sink(num_collect::logging::sinks::create_single_file_sink(
                "num_collect_bench_logging_write_trace_logs.log"))
            .output_log_level(num_collect::logging::log_level::iteration));

    perform();
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("trace_logs", "write log") {
    num_collect::logging::log_config::instance().set_default_tag_config(
        num_collect::logging::log_tag_config()
            .sink(num_collect::logging::sinks::create_single_file_sink(
                "num_collect_bench_logging_write_trace_logs.log"))
            .output_log_level(num_collect::logging::log_level::trace));

    perform();
}
