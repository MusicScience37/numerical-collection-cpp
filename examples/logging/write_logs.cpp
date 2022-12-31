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
 * \brief Example to write logs.
 */
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include <lyra/cli.hpp>
#include <lyra/opt.hpp>

#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/sinks/async_logging_worker.h"
#include "num_collect/logging/sinks/simple_log_sink.h"

constexpr auto my_tag = num_collect::logging::log_tag_view("example_tag");

static void write_logs() {
    // Create a logger with a tag.
    const auto logger = num_collect::logging::logger(my_tag);
    logger.info()("Example of logs with various log levels.");

    // Write logs.
    logger.trace()("trace");
    logger.debug()("debug");
    logger.summary()("summary");
    logger.info()("info");
    logger.warning()("warning");
    logger.error()("error");
    logger.critical()("critical");

    // These may not be used in ordinary user code.
    logger.iteration()("iteration");
    logger.iteration_label()("iteration_label");
}

static void write_to_default_tag() {
    // Create a logger without tag. (Default tag will be used.)
    const auto logger = num_collect::logging::logger();
    logger.info()("Example of logs without a log tag.");

    // Write logs.
    logger.trace()("trace");  // Not shown with the default configuration.
    logger.warning()("warning");
    logger.error()("error");
}

static void write_iterations() {
    // Logger.
    auto logger = num_collect::logging::logger(my_tag);
    logger.info()("Example of logs of iterations.");

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
    iteration_logger.write_iteration();

    // Iteratively set and write values.
    constexpr int repetition = 300;
    iteration_logger.start(logger);
    for (int i = 0; i < repetition; ++i) {
        val1 = i;
        iteration_logger.write_iteration();
    }

    // Last state.
    iteration_logger.write_summary();
}

auto main(int argc, char** argv) -> int {
    try {
        // Parse command line arguments.
        std::string config_filepath;
        const auto cli = lyra::cli().add_argument(
            lyra::opt(config_filepath, "path")
                .name("--config")
                .name("-c")
                .optional()
                .help("Load a logging configuration file."));
        const auto result = cli.parse({argc, argv});
        if (!result) {
            std::cerr << result.message() << "\n\n";
            std::cerr << cli << std::endl;
            return 1;
        }

        // Configure logging.
        if (config_filepath.empty()) {
            // Use custom configuration to show all logs.
            const auto config =
                num_collect::logging::log_config::instance()
                    .get_default_tag_config()
                    .output_log_level(num_collect::logging::log_level::trace);
            num_collect::logging::log_config::instance().set_default_tag_config(
                config);
        } else {
            // Use the give configuration file.
            num_collect::logging::load_logging_config_file(config_filepath);
        }

        write_logs();
        write_to_default_tag();
        write_iterations();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception thrown: " << e.what() << std::endl;
        return 1;
    }
}
