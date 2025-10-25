/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Common functions for diagrams.
 */
#include "fixed_step_bench_executor.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>
#include <fmt/core.h>
#include <fmt/format.h>
#include <msgpack_light/serialize.h>
#include <msgpack_light/type_support/struct.h>
#include <plotly_plotter/data_table.h>
#include <plotly_plotter/figure_builders/line.h>
#include <plotly_plotter/write_html.h>
#include <plotly_plotter/write_png.h>

#include "diagram_common.h"
#include "gzip_msgpack_output_stream.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_mixin.h"

MSGPACK_LIGHT_STRUCT_MAP(fixed_step_bench_executor::bench_result, solver_list,
    step_size_list, error_rate_list, energy_change_list, time_list);

fixed_step_bench_executor::fixed_step_bench_executor()
    : num_collect::logging::logging_mixin(benchmark_tag),
      iter_logger_(this->logger()) {
    iter_logger_.append("Solver", solver_name_)->width(20);
    iter_logger_.append("Step Size", step_size_);
    iter_logger_.append("Steps", steps_);
    iter_logger_.append<double>(
        "Time [us]", [this] { return mean_processing_time_sec_ * 1e+6; });
    iter_logger_.append("Error Rate", error_rate_);
    iter_logger_.append("Energy Change", energy_change_);
}

void fixed_step_bench_executor::write_result(std::string_view problem_name,
    std::string_view problem_description, std::string_view output_directory) {
    this->logger().info()("Write results.");

    const std::string solver_key = "Solver";
    const std::string step_size_key = "Step Size";
    const std::string error_key = "Error Rate";
    const std::string energy_change_key = "Energy Change";
    const std::string time_key = "Time [sec]";

    plotly_plotter::data_table data;
    data.emplace(solver_key, result_.solver_list);
    data.emplace(step_size_key, result_.step_size_list);
    data.emplace(error_key, result_.error_rate_list);
    data.emplace(energy_change_key, result_.energy_change_list);
    data.emplace(time_key, result_.time_list);

    const auto line_dash_map = create_line_dash_map();

    auto fig = plotly_plotter::figure_builders::line(data)
                   .x(time_key)
                   .y(error_key)
                   .group(solver_key)
                   .dash_map(line_dash_map)
                   .hover_data({step_size_key})
                   .mode("markers+lines")
                   .log_x(true)
                   .log_y(true)
                   .title(fmt::format(
                       "Work-Error Diagram of {}.", problem_description))
                   .create();

    const std::string base_name =
        fmt::format("{}/diagrams/{}", output_directory, problem_name);
    std::filesystem::create_directories(
        std::filesystem::path(base_name).parent_path());

    plotly_plotter::write_html(fmt::format("{}_err.html", base_name), fig);
    if (plotly_plotter::is_png_supported()) {
        plotly_plotter::write_png(fmt::format("{}_err.png", base_name), fig);
    }

    fig = plotly_plotter::figure_builders::line(data)
              .x(time_key)
              .y(energy_change_key)
              .group(solver_key)
              .dash_map(line_dash_map)
              .hover_data({step_size_key, error_key})
              .mode("markers+lines")
              .log_x(true)
              .log_y(true)
              .title(fmt::format(
                  "Work-Energy Change Diagram of {}.", problem_description))
              .create();

    plotly_plotter::write_html(
        fmt::format("{}_energy_change.html", base_name), fig);
    if (plotly_plotter::is_png_supported()) {
        plotly_plotter::write_png(
            fmt::format("{}_energy_change.png", base_name), fig);
    }

    this->logger().info()("Wrote results to {}.", base_name);

    gzip_msgpack_output_stream stream{
        fmt::format("{}/diagrams/{}.data", output_directory, problem_name)};
    msgpack_light::serialize_to(stream, result_);
}
