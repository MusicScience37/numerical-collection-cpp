/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of function_value_history_writer class.
 */
#include "function_value_history_writer.h"

#include <algorithm>
#include <iostream>

#include <msgpack_light/serialize.h>
#include <msgpack_light/type_support/optional.h>
#include <msgpack_light/type_support/struct.h>
#include <msgpack_light/type_support/vector.h>
#include <stat_bench/benchmark_macros.h>

#include "gzip_msgpack_output_stream.h"

MSGPACK_LIGHT_STRUCT_MAP(function_value_history_writer::measurement,
    problem_name, optimizer_name, evaluations, function_values,
    evaluations_upper, function_values_upper);

auto function_value_history_writer::instance()
    -> function_value_history_writer& {
    static function_value_history_writer instance;
    return instance;
}

void function_value_history_writer::write_measurements(
    const std::string& file_path) const {
    gzip_msgpack_output_stream output_stream(file_path);
    msgpack_light::serialize_to(output_stream, measurements_);
}

function_value_history_writer::function_value_history_writer() = default;

auto function_value_history_writer::has_measurement_of(
    const std::string& problem_name, const std::string& optimizer_name) const
    -> bool {
    return std::ranges::any_of(measurements_, [&](const auto& measurement) {
        return measurement.problem_name == problem_name &&
            measurement.optimizer_name == optimizer_name;
    });
}

auto main_with_function_value_history_writer(int argc, const char** argv)
    -> int {
    try {
        stat_bench::runner::CommandLineParser parser;
        std::string history_file_path;
        parser.cli().add_argument(lyra::opt(history_file_path, "path")
                .name("--history")
                .optional()
                .help("File path to write history of function values."));
        parser.parse_cli(argc, argv);
        if (parser.config().show_help) {
            std::cout << parser.cli()
                      << std::endl;  // NOLINT(performance-avoid-endl)
            return EXIT_SUCCESS;
        }

        stat_bench::runner::Runner runner{parser.config()};
        runner.run();

        if (!history_file_path.empty()) {
            function_value_history_writer::instance().write_measurements(
                history_file_path);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what()
                  << std::endl;  // NOLINT(performance-avoid-endl)
        return EXIT_FAILURE;
    }
}
