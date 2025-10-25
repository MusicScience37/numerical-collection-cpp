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
#include <string>

#include <stat_bench/benchmark_macros.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config_node.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/sinks/file_log_sink.h"

STAT_BENCH_MAIN

static void perform() {
    num_collect::logging::logger logger;
    STAT_BENCH_MEASURE_INDEXED(/*thread_ind*/, /*sample_ind*/, i) {
        NUM_COLLECT_LOG_TRACE(logger, "Test trace logs. i={}", i);
    };
}

STAT_BENCH_CASE("trace_logs", "write no log") {
    num_collect::logging::edit_default_log_tag_config()
        .sink(num_collect::logging::sinks::create_single_file_sink(
            "num_collect_bench_logging_write_trace_logs.log"))
        .output_log_level(num_collect::logging::log_level::iteration);

    perform();
}

STAT_BENCH_CASE("trace_logs", "write log") {
    num_collect::logging::edit_default_log_tag_config()
        .sink(num_collect::logging::sinks::create_single_file_sink(
            "num_collect_bench_logging_write_trace_logs.log"))
        .output_log_level(num_collect::logging::log_level::trace);

    perform();
}
