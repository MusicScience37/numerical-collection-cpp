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
#include <fmt/core.h>
#include <lyra/lyra.hpp>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"

auto main(int argc, char** argv) -> int {
    pybind11::scoped_interpreter interpreter;
    try {
        num_collect::index_type num_nodes = 200;  // NOLINT
        const auto cli =
            lyra::cli().add_argument(lyra::opt(num_nodes, "Number")
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

        std::vector<double> x_list;
        std::vector<double> y_list;
        x_list.reserve(nodes.size());
        y_list.reserve(nodes.size());
        for (const auto& node : nodes) {
            x_list.push_back(node.x());
            y_list.push_back(node.y());
        }
        std::unordered_map<std::string, pybind11::object> data;
        data.try_emplace("x", pybind11::cast(x_list));
        data.try_emplace("y", pybind11::cast(y_list));

        pybind11::module::import("pandas");
        auto px = pybind11::module::import("plotly.express");
        auto fig = px.attr("scatter")(pybind11::arg("data_frame") = data,
            pybind11::arg("x") = "x", pybind11::arg("y") = "y",
            pybind11::arg("title") =
                fmt::format("{} Halton nodes in 2D", num_nodes));

        fig.attr("write_html")("2d_halton_nodes.html");
        fig.attr("write_image")("2d_halton_nodes.png");
    } catch (const std::exception& e) {
        std::cerr << "Exception thrown: " << e.what() << std::endl;  // NOLINT
        return 1;
    }
}
