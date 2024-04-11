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

#include <exception>
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
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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
    iter_logger_.append("Solver", solver_name_)->width(20);  // NOLINT
    iter_logger_.append("Step Size", step_size_);
    iter_logger_.append("Steps", steps_);
    iter_logger_.append<double>("Time [us]", [this] {
        return mean_processing_time_sec_ * 1e+6;  // NOLINT
    });
    iter_logger_.append("Error Rate", error_rate_);
    iter_logger_.append("Energy Change", energy_change_);
}

void fixed_step_bench_executor::write_result(std::string_view problem_name,
    std::string_view problem_description, std::string_view output_directory) {
    this->logger().info()("Write results.");

    static pybind11::scoped_interpreter interpreter;
    pybind11::gil_scoped_acquire gil;
    try {
        auto pd = pybind11::module::import("pandas");
        auto px = pybind11::module::import("plotly.express");

        const std::string solver_key = "Solver";
        const std::string step_size_key = "Step Size";
        const std::string error_key = "Error Rate";
        const std::string energy_change_key = "Energy Change";
        const std::string time_key = "Time [sec]";

        std::unordered_map<std::string, pybind11::object> data;
        data.try_emplace(solver_key, pybind11::cast(result_.solver_list));
        data.try_emplace(step_size_key, pybind11::cast(result_.step_size_list));
        data.try_emplace(error_key, pybind11::cast(result_.error_rate_list));
        data.try_emplace(
            energy_change_key, pybind11::cast(result_.energy_change_list));
        data.try_emplace(time_key, pybind11::cast(result_.time_list));

        const auto line_dash_map = create_line_dash_map();

        auto fig = px.attr("line")(              //
            pybind11::arg("data_frame") = data,  //
            pybind11::arg("x") = time_key,
            pybind11::arg("y") = error_key,                  //
            pybind11::arg("color") = solver_key,             //
            pybind11::arg("line_dash") = solver_key,         //
            pybind11::arg("line_dash_map") = line_dash_map,  //
            pybind11::arg("hover_data") = std::vector<std::string>{solver_key,
                step_size_key, error_key, time_key},  //
            pybind11::arg("markers") = true,          //
            pybind11::arg("log_x") = true,            //
            pybind11::arg("log_y") = true,            //
            pybind11::arg("title") =
                fmt::format("Work-Error Diagram of {}.", problem_description));

        const std::string base_name =
            fmt::format("{}/diagrams/{}", output_directory, problem_name);
        std::filesystem::create_directories(
            std::filesystem::path(base_name).parent_path());

        fig.attr("write_html")(fmt::format("{}_err.html", base_name));
        fig.attr("write_image")(fmt::format("{}_err.png", base_name));

        fig = px.attr("line")(                   //
            pybind11::arg("data_frame") = data,  //
            pybind11::arg("x") = time_key,
            pybind11::arg("y") = energy_change_key,          //
            pybind11::arg("color") = solver_key,             //
            pybind11::arg("line_dash") = solver_key,         //
            pybind11::arg("line_dash_map") = line_dash_map,  //
            pybind11::arg("hover_data") = std::vector<std::string>{solver_key,
                step_size_key, error_key, time_key},  //
            pybind11::arg("markers") = true,          //
            pybind11::arg("log_x") = true,            //
            pybind11::arg("log_y") = true,            //
            pybind11::arg("title") = fmt::format(
                "Work-Energy Change Diagram of {}.", problem_description));

        fig.attr("write_html")(fmt::format("{}_energy_change.html", base_name));
        fig.attr("write_image")(fmt::format("{}_energy_change.png", base_name));

        this->logger().info()("Wrote results to {}.", base_name);
    } catch (const std::exception& e) {
        this->logger().error()("Exception in writing the result: {}", e.what());
        PyErr_Clear();
    }

    gzip_msgpack_output_stream stream{
        fmt::format("{}/diagrams/{}.data", output_directory, problem_name)};
    msgpack_light::serialize_to(stream, result_);
}
