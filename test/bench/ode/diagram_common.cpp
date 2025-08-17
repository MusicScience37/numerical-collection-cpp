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
#include "diagram_common.h"

#include <atomic>
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

#include "gzip_msgpack_output_stream.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_config_node.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/step_size_limits.h"

MSGPACK_LIGHT_STRUCT_MAP(bench_executor::bench_result, solver_list,
    tolerance_list, error_rate_list, time_list);

void prevent_ordering() { std::atomic_signal_fence(std::memory_order_seq_cst); }

auto create_line_dash_map() -> std::unordered_map<std::string, std::string> {
    return std::unordered_map<std::string, std::string>{
        // Explicit Runge-Kutta.
        {"RKF45", "solid"}, {"DOPRI5", "solid"}, {"ARK4(3)-ERK", "solid"},
        {"RK4", "solid"},
        // Implicit Runge-Kutta.
        {"Tanaka1", "dot"}, {"Tanaka2", "dot"}, {"SDIRK4", "dot"},
        {"ARK4(3)-ESDIRK", "dot"}, {"ARK5(4)-ESDIRK", "dot"},
        {"ESDIRK45c", "dot"},
        // Rosenbrock.
        {"ROS3w", "longdash"}, {"ROS34PW3", "longdash"}, {"RODASP", "longdash"},
        {"RODASPR", "longdash"},
        // AVF.
        {"AVF2", "dashdot"}, {"AVF3", "dashdot"}, {"AVF4", "dashdot"},
        // Symplectic.
        {"LeapFrog", "longdash"}, {"Forest4", "longdash"},
        // LU in Rosenbrock.
        {"ROS3w_lu", "solid"}, {"ROS34PW3_lu", "solid"}, {"RODASP_lu", "solid"},
        // Broyden in Rosenbrock.
        {"ROS3w_broyden", "longdash"}, {"ROS34PW3_broyden", "longdash"},
        // GMRES in Rosenbrock.
        {"ROS3w_gmres", "dashdot"}, {"ROS34PW3_gmres", "dashdot"},
        {"RODASP_gmres", "dashdot"},
        // BiCGstab in Rosenbrock.
        {"ROS3w_bicgstab", "dot"}, {"ROS34PW3_bicgstab", "dot"},
        {"RODASP_bicgstab", "dot"},
        // Basic step size controller.
        {"RKF45_basic", "solid"}, {"ROS3w_basic", "solid"},
        {"RODASP_basic", "solid"},
        // PI step size controller.
        {"RKF45_pi", "dot"}, {"ROS3w_pi", "dot"}, {"RODASP_pi", "dot"},
        //
    };
}

bench_executor::bench_executor()
    : num_collect::logging::logging_mixin(benchmark_tag),
      iter_logger_(this->logger()) {
    iter_logger_.append("Solver", solver_name_)->width(20);  // NOLINT
    iter_logger_.append("Tol.", tol_);
    iter_logger_.append("Steps", steps_);
    iter_logger_.append<double>("Time [us]", [this] {
        return mean_processing_time_sec_ * 1e+6;  // NOLINT
    });
    iter_logger_.append("Error Rate", error_rate_);
}

void bench_executor::write_result(std::string_view problem_name,
    std::string_view problem_description, std::string_view output_directory) {
    this->logger().info()("Write results.");

    const std::string solver_key = "Solver";
    const std::string tolerance_key = "Err. Tol.";
    const std::string error_key = "Error Rate";
    const std::string time_key = "Time [sec]";

    plotly_plotter::data_table data;
    data.emplace(solver_key, result_.solver_list);
    data.emplace(tolerance_key, result_.tolerance_list);
    data.emplace(error_key, result_.error_rate_list);
    data.emplace(time_key, result_.time_list);

    const auto line_dash_map = create_line_dash_map();

    auto fig = plotly_plotter::figure_builders::line(data)
                   .x(time_key)
                   .y(error_key)
                   .group(solver_key)
                   .dash_map(line_dash_map)
                   .hover_data({tolerance_key})
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

    plotly_plotter::write_html(fmt::format("{}.html", base_name), fig);
    if (plotly_plotter::is_png_supported()) {
        plotly_plotter::write_png(fmt::format("{}.png", base_name), fig);
    }

    this->logger().info()("Wrote results to {}.", base_name);

    gzip_msgpack_output_stream stream{
        fmt::format("{}/diagrams/{}.data", output_directory, problem_name)};
    msgpack_light::serialize_to(stream, result_);
}

void bench_executor::step_size_limits(
    const num_collect::ode::step_size_limits<double>& limits) {
    step_size_limits_ = limits;
}

void configure_logging() {
    num_collect::logging::edit_default_log_tag_config().output_log_level(
        num_collect::logging::log_level::info);

    num_collect::logging::edit_log_tag_config(benchmark_tag)
        .output_log_level(num_collect::logging::log_level::iteration)
        .iteration_output_period(1);
}
