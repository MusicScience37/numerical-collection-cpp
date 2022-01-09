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
#include <iostream>
#include <memory>

#include "num_collect/logging/iteration_logger.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"

constexpr auto my_tag = num_collect::logging::log_tag_view("example tag");

static void write_logs() {
    // Configuration.
    const auto config = num_collect::logging::log_config::instance()
                            .get_default_tag_config()
                            .write_traces(true)
                            .write_iterations(true)
                            .write_summary(true);
    num_collect::logging::log_config::instance().set_config_of(my_tag, config);

    // Create a logger with a tag.
    const auto logger = num_collect::logging::logger(my_tag);

    // Write logs.
    logger.trace()("trace");
    logger.summary()("summary");
    logger.info()("info");
    logger.warning()("warning");
    logger.error()("error");

    // These may not be used in ordinary user code.
    logger.iteration()("iteration");
    logger.iteration_label()("iteration_label");
}

static void write_to_default_tag() {
    // Create a logger without tag. (Default tag will be used.)
    const auto logger = num_collect::logging::logger();

    // Write logs.
    logger.trace()("trace");  // Not shown with the default configuration.
    logger.warning()("warning");
    logger.error()("error");
}

static void write_iterations() {
    // Configuration.
    const auto config = num_collect::logging::log_config::instance()
                            .get_default_tag_config()
                            .write_traces(true)
                            .write_iterations(true)
                            .write_summary(true)
                            .iteration_output_period(2)  // NOLINT
                            .iteration_label_period(5);  // NOLINT
    num_collect::logging::log_config::instance().set_config_of(my_tag, config);

    // Logger.
    const auto logger = num_collect::logging::logger(my_tag);

    // Configure.
    auto iteration_logger = num_collect::logging::iteration_logger();
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
    iteration_logger.write_iteration_to(logger);

    // Iteratively set and write values.
    constexpr int repetition = 20;
    iteration_logger.reset_count();
    for (int i = 0; i < repetition; ++i) {
        val1 = i;
        iteration_logger.write_iteration_to(logger);
    }

    // Last state.
    iteration_logger.write_summary_to(logger);
}

auto main(int argc, char** argv) -> int {
    try {
        if (argc == 2) {
            // Configure logging to a file.
            const std::string log_file_path = argv[1];  // NOLINT
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
