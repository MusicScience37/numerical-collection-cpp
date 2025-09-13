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
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <fmt/ranges.h>

#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"

constexpr auto my_tag = num_collect::logging::log_tag_view("example_tag");

static void write_logs() {
    // Create a logger with a tag.
    const auto logger = num_collect::logging::logger(my_tag);

    // Write logs.
    logger.trace()("trace");
    logger.debug()("debug");
    logger.iteration()("iteration");
    logger.iteration_label()("iteration_label");
    logger.summary()("summary");
    logger.info()("info");
    logger.warning()("warning");
    logger.error()("error");
    logger.critical()("critical");
}

static void write_to_default_tag() {
    // Create a logger without tag. (Default tag will be used.)
    const auto logger = num_collect::logging::logger();

    // Write logs.
    logger.trace()("trace");
    logger.debug()("debug");
    logger.iteration()("iteration");
    logger.iteration_label()("iteration_label");
    logger.summary()("summary");
    logger.info()("info");
    logger.warning()("warning");
    logger.error()("error");
    logger.critical()("critical");
}

static void write_logs_with_macros() {
    // Create a logger with a tag.
    const auto logger = num_collect::logging::logger(my_tag);

    // Write logs.
    NUM_COLLECT_LOG_TRACE(logger, "trace");
    NUM_COLLECT_LOG_DEBUG(logger, "debug");
    NUM_COLLECT_LOG_ITERATION(logger, "iteration");
    NUM_COLLECT_LOG_ITERATION_LABEL(logger, "iteration_label");
    NUM_COLLECT_LOG_SUMMARY(logger, "summary");
    NUM_COLLECT_LOG_INFO(logger, "info");
    NUM_COLLECT_LOG_WARNING(logger, "warning");
    NUM_COLLECT_LOG_ERROR(logger, "error");
    NUM_COLLECT_LOG_CRITICAL(logger, "critical");
}

static void write_iterations() {
    // Logger.
    auto logger = num_collect::logging::logger(my_tag);

    // Configure.
    auto iteration_logger =
        num_collect::logging::iterations::iteration_logger(logger);
    int val1 = 0;
    iteration_logger.append("val1", val1);  // Reference is hold here.
    std::string val2;
    iteration_logger.append("val2", val2);
    iteration_logger.template append<double>("val3", [] {
        // Use a function to return the value.
        return 1.23456;  // NOLINT
    });

    // Set and write values.
    val1 = 3;  // NOLINT
    val2 = "abc";

    // Iteratively set and write values.
    constexpr int repetition = 123;
    for (int i = 0; i < repetition; ++i) {
        val1 = i;
        iteration_logger.write_iteration();
    }

    // Last state.
    iteration_logger.write_summary();
}

static void write_parameters() {
    // Logger.
    auto logger = num_collect::logging::logger(my_tag);

    constexpr int param1 = 12345;
    logger.debug()("Write a parameter: {}.", param1);

    const std::vector<double> param2{1.234, 5.678};
    const std::string_view param3 = "abc";
    logger.info()("Write multiple parameters: [{:.3f}], {}",
        fmt::join(param2, ","), param3);
}

auto main(int argc, char** argv) -> int {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <configuration-file>"
                      << std::endl;
            return 1;
        }

        const std::string_view config_filepath = argv[1];
        num_collect::logging::load_logging_config_file(
            std::string{config_filepath});

        write_logs();
        write_to_default_tag();
        write_logs_with_macros();
        write_iterations();
        write_parameters();

        // In MSVC, wait for a second to write logs.
#ifdef _MSC_VER
        std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception thrown: " << e.what() << std::endl;
        return 1;
    }
}
