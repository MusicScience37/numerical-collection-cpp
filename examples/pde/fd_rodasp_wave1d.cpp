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
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Eigen/Core>
#include <fmt/format.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>
#include <plotly_plotter/write_png.h>
#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
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
        NUM_COLLECT_LOG_AND_THROW(num_collect::invalid_argument,
            "Failed to load configuration {}.", key);
    }
    return res.value();
}

auto main(int argc, char** argv) -> int {
    std::string_view config_filepath = "examples/pde/fd_rodasp_wave1d.toml";
    if (argc == 2) {
        config_filepath = argv[1];
    }
    num_collect::logging::load_logging_config_file(
        std::string(config_filepath));

    using problem_type = num_prob_collect::ode::string_wave_1d_problem;
    using solver_type =
        num_collect::ode::rosenbrock::rodasp_solver<problem_type>;

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

    plotly_plotter::figure figure;

    auto trace = figure.add_scatter();
    trace.x(problem.points());
    trace.y(solver.variable().tail(problem.points().size()));
    trace.mode("lines");
    trace.name(fmt::format("t = {:.1f}", init_time));

    const auto time_list = std::vector<double>{0.2, 0.4, 0.6, 0.8, 1.0};
    for (const double time : time_list) {
        NUM_COLLECT_LOG_TRACE(logger, "Solve till {}", time);
        solver.solve_till(time);

        auto trace = figure.add_scatter();
        trace.x(problem.points());
        trace.y(solver.variable().tail(problem.points().size()));
        trace.mode("lines");
        trace.name(fmt::format("t = {:.1f}", init_time));

        solution.evaluate_on(time);
        const auto error_norm =
            (solver.variable() - solution.solution()).norm();
        logger.info()("Error norm at time {:.3f}: {:.3e}", time, error_norm);
    }

    figure.title(
        "Solution of 1D wave equation using finite difference and "
        "Rosenbrock method (RODASP formula)");
    figure.layout().xaxis().title().text("x");
    figure.layout().yaxis().title().text("Displacement");

    plotly_plotter::write_html("fd_rodasp_wave1d.html", figure);
    if (plotly_plotter::is_png_supported()) {
        plotly_plotter::write_png("fd_rodasp_wave1d.png", figure);
    }

    return 0;
}
