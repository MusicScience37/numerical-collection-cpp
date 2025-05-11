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
 * \brief Example to generate Halton nodes in 2D.
 */
#include <iostream>
#include <string>

#include <Eigen/Core>
#include <fmt/format.h>
#include <lyra/lyra.hpp>
#include <plotly_plotter/data_table.h>
#include <plotly_plotter/figure_builders/scatter.h>
#include <plotly_plotter/write_html.h>
#include <plotly_plotter/write_png.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"

auto main(int argc, char** argv) -> int {
    num_collect::index_type num_nodes = 200;  // NOLINT
    const auto cli = lyra::cli().add_argument(lyra::opt(num_nodes, "Number")
            .name("--num_nodes")
            .name("-n")
            .optional()
            .help("Set the number of nodes."));
    const auto result = cli.parse({argc, argv});
    if (!result) {
        std::cerr << result.message() << "\n\n";
        std::cerr << cli << std::endl;  // NOLINT
        return 1;
    }

    const auto nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_nodes);

    plotly_plotter::data_table data;
    auto x_list = data.emplace<double>("x");
    auto y_list = data.emplace<double>("y");
    for (const auto& node : nodes) {
        x_list->push_back(node.x());
        y_list->push_back(node.y());
    }

    const auto figure =
        plotly_plotter::figure_builders::scatter(data)
            .x("x")
            .y("y")
            .title(fmt::format("{} Halton nodes in 2D", num_nodes))
            .create();

    plotly_plotter::write_html("2d_halton_nodes.html", figure);
    plotly_plotter::write_png("2d_halton_nodes.png", figure);
}
