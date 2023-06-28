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
 * \brief Benchmark of logging with iteration_logger class.
 */
#include "num_collect/logging/iterations/iteration_logger.h"

#include <string>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/sinks/simple_log_sink.h"

STAT_BENCH_MAIN

static constexpr auto tag = num_collect::logging::log_tag_view("benchmark");

static void perform() {
    auto logger = num_collect::logging::logger(tag);
    auto iteration_logger =
        num_collect::logging::iterations::iteration_logger(logger);

    const auto val1_func = [] { return 3.14159265; };  // NOLINT
    iteration_logger.template append<double>("val1", val1_func);
    const auto val2_func = [] { return std::string("test"); };
    iteration_logger.template append<std::string>("val2", val2_func);
    const auto val3_func = [] {
        return num_collect::index_type(12345);  // NOLINT
    };
    iteration_logger.template append<num_collect::index_type>(
        "val2", val3_func);

    STAT_BENCH_MEASURE() { iteration_logger.write_iteration(); };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("iteration_logger", "write no log") {
    const std::string log_file_path =
        "num_collect_bench_logging_iteration_logger.log";
    const auto config = num_collect::logging::log_tag_config().sink(
        num_collect::logging::sinks::create_single_file_sink(log_file_path));
    num_collect::logging::set_config_of(tag, config);

    perform();
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("iteration_logger", "write log") {
    const std::string log_file_path =
        "num_collect_bench_logging_iteration_logger.log";
    const auto config =
        num_collect::logging::log_tag_config()
            .sink(num_collect::logging::sinks::create_single_file_sink(
                log_file_path))
            .output_log_level(num_collect::logging::log_level::trace);
    num_collect::logging::set_config_of(tag, config);

    perform();
}
