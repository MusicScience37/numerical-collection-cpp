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
 * \brief Test of log sinks.
 */
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "num_collect/base/concepts/invocable.h"
#include "num_collect/logging/config/toml/toml_log_config_parser.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_mixin.h"

class bench_executor : public num_collect::logging::logging_mixin {
public:
    bench_executor()
        : num_collect::logging::logging_mixin(num_collect::logging::log_tag(
              "num_collect_bench_logging_log_sinks")),
          iter_logger_(this->logger()) {
        iter_logger_.append("Sink", log_sink_name_)->width(25);  // NOLINT
        iter_logger_.append("Log", log_type_name_)->width(15);   // NOLINT
        iter_logger_.append<double>("Time [us]", [this] {
            return cum_mean_time_sec_ * 1e+6  // NOLINT
                / static_cast<double>(samples);
        });
    }

    void measure(std::string_view log_tag_str, std::string_view display_name) {
        const auto log_tag = num_collect::logging::log_tag{log_tag_str};
        const auto logger = num_collect::logging::logger{log_tag};

        measure_impl([&logger] { logger.debug()("Logging test."); },
            "short string", display_name);

        const std::string long_string = std::string(200, 'a');
        measure_impl([&logger, long_string] { logger.debug()(long_string); },
            "long string", display_name);
    }

    void write_result(std::string_view output_directory) {
        this->logger().info()("Write results.");

        static pybind11::scoped_interpreter interpreter;
        pybind11::gil_scoped_acquire gil;

        auto pd = pybind11::module::import("pandas");
        auto px = pybind11::module::import("plotly.express");

        const std::string log_sink_name_key = "Log Sink";
        const std::string log_type_name_key = "Type of Logs";
        const std::string mean_time_sec_key = "Time [sec]";

        std::unordered_map<std::string, pybind11::object> data;
        data.try_emplace(
            log_sink_name_key, pybind11::cast(log_sink_name_list_));
        data.try_emplace(
            log_type_name_key, pybind11::cast(log_type_name_list_));
        data.try_emplace(
            mean_time_sec_key, pybind11::cast(mean_time_sec_list_));

        auto fig = px.attr("violin")(                    //
            pybind11::arg("data_frame") = data,          //
            pybind11::arg("x") = log_type_name_key,      //
            pybind11::arg("y") = mean_time_sec_key,      //
            pybind11::arg("color") = log_sink_name_key,  //
            pybind11::arg("box") = true,                 //
            pybind11::arg("log_y") = true,               //
            pybind11::arg("title") = "Time to Write Logs in Caller Thread");

        const double log_time_lower_limit =
            std::log10(*std::min_element(mean_time_sec_list_.begin(),
                           mean_time_sec_list_.end()) /
                1.5);  // NOLINT
        const double log_time_upper_limit =
            std::log10(*std::max_element(mean_time_sec_list_.begin(),
                           mean_time_sec_list_.end()) *
                1.5);  // NOLINT
        fig.attr("update_layout")(pybind11::dict(
            pybind11::arg("yaxis") =
                pybind11::dict(pybind11::arg("constrain") = "range",  //
                    pybind11::arg("range") =
                        std::vector{fmt::format("{}", log_time_lower_limit),
                            fmt::format("{}", log_time_upper_limit)})));

        const std::string base_name = fmt::format(
            "{}/num_collect_bench_logging_log_sinks", output_directory);
        std::filesystem::create_directories(
            std::filesystem::path(base_name).parent_path());

        fig.attr("write_html")(fmt::format("{}.html", base_name));
        fig.attr("write_image")(fmt::format("{}.png", base_name));

        this->logger().info()("Wrote results to {}.", base_name);
    }

private:
    template <num_collect::concepts::invocable<> Function>
    void measure_impl(Function&& logging_function,
        std::string_view log_type_name, std::string_view log_sink_name) {
        log_sink_name_ = log_sink_name;
        log_type_name_ = log_type_name;

        logging_function();

        cum_mean_time_sec_ = 0.0;
        for (std::size_t s = 0; s < samples; ++s) {
            const auto start = std::chrono::steady_clock::now();

            std::atomic_signal_fence(std::memory_order::seq_cst);
            for (std::size_t i = 0; i < iterations; ++i) {
                logging_function();
            }
            std::atomic_signal_fence(std::memory_order::seq_cst);

            const auto end = std::chrono::steady_clock::now();

            const auto duration = end - start;
            mean_time_sec_ =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    duration)
                    .count() /
                static_cast<double>(iterations);

            log_sink_name_list_.emplace_back(log_sink_name);
            log_type_name_list_.emplace_back(log_type_name);
            mean_time_sec_list_.push_back(mean_time_sec_);
            cum_mean_time_sec_ += mean_time_sec_;

            std::this_thread::sleep_for(cool_time);
        }

        iter_logger_.write_iteration();
    }

    static constexpr std::size_t iterations = 100;
    static constexpr std::size_t samples = 100;
    static constexpr auto cool_time = std::chrono::milliseconds(10);

    num_collect::logging::iterations::iteration_logger<> iter_logger_;
    std::string log_sink_name_{};
    std::string log_type_name_{};
    double mean_time_sec_{};
    double cum_mean_time_sec_{};

    std::vector<std::string> log_sink_name_list_{};
    std::vector<std::string> log_type_name_list_{};
    std::vector<double> mean_time_sec_list_{};
};

static void configure_logger(std::string_view output_directory) {
    const auto config_text = fmt::format(R"(
[[num_collect.logging.tag_configs]]
tag = "num_collect_bench_logging_log_sinks"
sink = "bench_log_sink"
output_log_level = "trace"
iteration_output_period = 1

[[num_collect.logging.sinks]]
name = "bench_log_sink"
type = "console"

[[num_collect.logging.tag_configs]]
tag = "single_file"
sink = "single_file"
output_log_level = "trace"

[[num_collect.logging.sinks]]
name = "single_file"
type = "single_file"
filepath = "{0}/temp/num_collect_bench_logging_log_sinks.log"

[[num_collect.logging.tag_configs]]
tag = "async_single_file"
sink = "async_single_file"
output_log_level = "trace"

[[num_collect.logging.sinks]]
name = "async_single_file"
type = "async"
inner_sink_name = "single_file"
)",
        output_directory);

    num_collect::logging::config::toml::toml_log_config_parser()
        .parse_from_text(config_text);
}

auto main(int argc, char** argv) -> int {
    try {
        if (argc != 2) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            fmt::print("Usage: {} <output_directory>", argv[0]);
            return 1;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        const std::string_view output_directory = argv[1];

        configure_logger(output_directory);

        bench_executor executor;
        executor.measure("single_file", "single_file sink");
        executor.measure("async_single_file", "async single_file sink");
        executor.write_result(output_directory);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception thrown: " << e.what() << std::endl;
    }
}
