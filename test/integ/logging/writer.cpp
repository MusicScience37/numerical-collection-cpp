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
#include <iostream>
#include <memory>

#include <lyra/lyra.hpp>

#include "num_collect/logging/iteration_logger.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/simple_log_sink.h"
#include "num_collect/util/source_info_view.h"

constexpr auto my_tag = num_collect::logging::log_tag_view("example tag");

static void write_logs() {
    // Location for test.
    const auto location = num_collect::util::source_info_view(
        "/test/logging.cpp", 1, 0, "write_logs");

    // Configuration.
    const auto config =
        num_collect::logging::log_config::instance()
            .get_default_tag_config()
            .output_log_level(num_collect::logging::log_level::trace);
    num_collect::logging::log_config::instance().set_config_of(my_tag, config);

    // Create a logger with a tag.
    const auto logger = num_collect::logging::logger(my_tag);

    // Write logs.
    logger.trace(location)("trace");
    logger.summary(location)("summary");
    logger.info(location)("info");
    logger.warning(location)("warning");
    logger.error(location)("error");

    // These may not be used in ordinary user code.
    logger.iteration(location)("iteration");
    logger.iteration_label(location)("iteration_label");
}

static void write_to_default_tag() {
    // Location for test.
    const auto location = num_collect::util::source_info_view(
        "/test/logging.cpp", 2, 0, "write_to_default_tag");

    // Create a logger without tag. (Default tag will be used.)
    const auto logger = num_collect::logging::logger();

    // Write logs.
    logger.trace(location)(
        "trace");  // Not shown with the default configuration.
    logger.warning(location)("warning");
    logger.error(location)("error");
}

static void write_iterations() {
    // Location for test.
    const auto location = num_collect::util::source_info_view(
        "/test/logging.cpp", 3, 0, "write_iterations");

    // Configuration.
    const auto config =
        num_collect::logging::log_config::instance()
            .get_default_tag_config()
            .output_log_level(num_collect::logging::log_level::trace)
            .iteration_output_period(2)  // NOLINT
            .iteration_label_period(5);  // NOLINT
    num_collect::logging::log_config::instance().set_config_of(my_tag, config);

    // Logger.
    auto logger = num_collect::logging::logger(my_tag);

    // Configure.
    auto iteration_logger = num_collect::logging::iteration_logger(logger);
    int val1 = 0;
    iteration_logger.append("val1", val1);  // Reference is hold here.
    std::string val2;
    iteration_logger.append("val2", val2);
    iteration_logger.append<double>("val3", [] {
        // Use a function to return the value.
        return 1.23456;  // NOLINT
    });

    // Set and write values.
    val1 = 3;  // NOLINT
    val2 = "abc";
    iteration_logger.write_iteration(location);

    // Iteratively set and write values.
    constexpr int repetition = 20;
    iteration_logger.reset_count();
    for (int i = 0; i < repetition; ++i) {
        val1 = i;
        iteration_logger.write_iteration(location);
    }

    // Last state.
    iteration_logger.write_summary(location);
}

auto main(int argc, char** argv) -> int {
    try {
        std::string log_file_path;
        auto cli = lyra::cli() |
            lyra::opt(log_file_path, "filepath")["-o"]["--out"](
                "Write logs to a file.");
        const auto result = cli.parse({argc, argv});
        if (!result) {
            std::cerr << cli << std::endl;
            return 1;
        }

        if (!log_file_path.empty()) {
            // Configure logging to a file.
            const auto config = num_collect::logging::log_tag_config().sink(
                std::make_shared<num_collect::logging::simple_log_sink>(
                    log_file_path));
            num_collect::logging::log_config::instance().set_default_tag_config(
                config);
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
