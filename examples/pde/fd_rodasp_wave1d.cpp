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
 * \brief Example of solving the wave equation using finite difference and
 * RODASP formula.
 */
#include <cmath>
#include <exception>
#include <iostream>
#include <string_view>

#include <Eigen/Core>
#include <fmt/format.h>
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/rosenbrock/bicgstab_rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/format_dense_vector.h"
#include "num_prob_collect/ode/string_wave_1d_problem.h"

template <typename T>
static auto get_config_value(
    const toml::parse_result& config, std::string_view key) {
    const std::optional<T> res = config.as_table()
                                     ->at("fd_rodasp_wave1d")
                                     .as_table()
                                     ->at(key)
                                     .value<T>();
    if (!res) {
        throw num_collect::invalid_argument(
            fmt::format("Failed to load configuration {}.", key));
    }
    return res.value();
}

auto main(int argc, char** argv) -> int {
    try {
        std::string_view config_filepath = "examples/pde/fd_rodasp_wave1d.toml";
        if (argc == 2) {
            config_filepath = argv[1];  // NOLINT
        }
        num_collect::logging::load_logging_config(std::string(config_filepath));

        using problem_type = num_prob_collect::ode::string_wave_1d_problem;
        using formula_type =
            num_collect::ode::rosenbrock::rodasp_formula<problem_type,
                num_collect::ode::rosenbrock::
                    bicgstab_rosenbrock_equation_solver<problem_type>>;
        using solver_type = num_collect::ode::embedded_solver<formula_type>;

        const auto config = toml::parse_file(config_filepath);
        const double speed = get_config_value<double>(config, "speed");
        const num_collect::index_type num_points =
            get_config_value<num_collect::index_type>(config, "num_points");
        const double length = get_config_value<double>(config, "length");

        const num_prob_collect::ode::string_wave_1d_parameters params{
            .speed = speed, .num_points = num_points, .length = length};

        problem_type problem{params};
        solver_type solver{problem};

        num_prob_collect::ode::string_wave_1d_solution solution{params};

        constexpr double init_time = 0.0;
        solution.evaluate_on(init_time);
        solver.init(init_time, solution.solution());

        num_collect::logging::logger logger;

        pybind11::scoped_interpreter interpreter;
        auto go = pybind11::module::import("plotly.graph_objects");
        auto fig = go.attr("Figure")();

        fig.attr("add_trace")(
            go.attr("Scatter")(pybind11::arg("x") = problem.points(),
                pybind11::arg("y") =
                    solver.variable().tail(problem.points().size()),
                pybind11::arg("mode") = "lines",
                pybind11::arg("name") = fmt::format("t = {:.1f}", 0.0)));

        const auto time_list = std::vector<double>{0.2, 0.4, 0.6, 0.8, 1.0};
        for (const double time : time_list) {
            logger.trace()("Solve till {}", time);
            solver.solve_till(time);

            fig.attr("add_trace")(
                go.attr("Scatter")(pybind11::arg("x") = problem.points(),
                    pybind11::arg("y") =
                        solver.variable().tail(problem.points().size()),
                    pybind11::arg("mode") = "lines",
                    pybind11::arg("name") = fmt::format("t = {:.1f}", time)));

            solution.evaluate_on(time);
            const auto error_norm =
                (solver.variable() - solution.solution()).norm();
            logger.info()(
                "Error norm at time {:.3f}: {:.3e}", time, error_norm);
        }

        fig.attr("update_layout")(
            pybind11::arg("title") =
                "Solution of 1D wave equation using finite difference and "
                "Rosenbrock method (RODASP formula)",
            pybind11::arg("xaxis_title") = "x",
            pybind11::arg("yaxis_title") = "Displacement");

        fig.attr("write_html")("fd_rodasp_wave1d.html");
        fig.attr("write_image")("fd_rodasp_wave1d.png");

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
