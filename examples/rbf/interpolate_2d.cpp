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
 * \brief Example to interpolate in 2D.
 */
#include <cstdio>
#include <exception>
#include <string_view>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <fmt/format.h>
#include <pybind11/eigen.h>  // IWYU pragma: keep
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // IWYU pragma: keep

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

static constexpr double x_min = 0.0;
static constexpr double x_max = 2.0;
static constexpr double y_min = -1.0;
static constexpr double y_max = 1.0;
static constexpr double c_min = 0.0;
static constexpr double c_max = 2.0;
static constexpr num_collect::index_type num_sample_points = 200;

[[nodiscard]] static auto target_function(const Eigen::Vector2d& variable)
    -> double {
    constexpr double slope = 0.5;
    return slope * variable.x() + variable.y() * variable.y();
}

[[nodiscard]] static auto convert_to_plotly_data(
    const std::vector<Eigen::Vector2d>& variables,
    const Eigen::VectorXd& values) {
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> value;
    x.reserve(static_cast<std::size_t>(num_sample_points));
    y.reserve(static_cast<std::size_t>(num_sample_points));
    value.reserve(static_cast<std::size_t>(num_sample_points));
    for (num_collect::index_type i = 0; i < num_sample_points; ++i) {
        x.push_back(variables[static_cast<std::size_t>(i)].x());
        y.push_back(variables[static_cast<std::size_t>(i)].y());
        value.push_back(values(i));
    }
    return pybind11::dict(pybind11::arg("x") = x, pybind11::arg("y") = y,
        pybind11::arg("value") = value);
}

static void save(const pybind11::object& fig, std::string_view name) {
    fig.attr("write_image")(fmt::format("rbf_{}.png", name));
}

auto main() -> int {
    pybind11::scoped_interpreter interpreter;
    try {
        pybind11::module::import("pandas");
        auto px = pybind11::module::import("plotly.express");
        auto go = pybind11::module::import("plotly.graph_objects");

        std::vector<Eigen::Vector2d> sample_variables =
            num_collect::rbf::generate_halton_nodes<double, 2>(
                num_sample_points);
        for (auto& variable : sample_variables) {
            variable.x() = x_min + (x_max - x_min) * variable.x();
            variable.y() = y_min + (y_max - y_min) * variable.y();
        }
        Eigen::VectorXd sample_values =
            Eigen::VectorXd::Zero(num_sample_points);
        for (num_collect::index_type i = 0; i < num_sample_points; ++i) {
            sample_values(i) =
                target_function(sample_variables[static_cast<std::size_t>(i)]);
        }

        {
            auto fig = px.attr("scatter")(
                convert_to_plotly_data(sample_variables, sample_values),
                pybind11::arg("x") = "x", pybind11::arg("y") = "y",
                pybind11::arg("color") = "value",
                pybind11::arg("range_color") = std::make_pair(c_min, c_max));
            save(fig, "sample_points");
        }

        num_collect::rbf::global_rbf_polynomial_interpolator<double(
            Eigen::Vector2d)>
            interpolator;
        interpolator.compute(sample_variables, sample_values);
        constexpr num_collect::index_type num_divisions = 100;
        Eigen::VectorXd contour_x =
            Eigen::VectorXd::LinSpaced(num_divisions + 1, x_min, x_max);
        Eigen::VectorXd contour_y =
            Eigen::VectorXd::LinSpaced(num_divisions + 1, y_min, y_max);
        Eigen::MatrixXd contour_c =
            Eigen::MatrixXd::Zero(num_divisions + 1, num_divisions + 1);
        for (num_collect::index_type i = 0; i <= num_divisions; ++i) {
            for (num_collect::index_type j = 0; j <= num_divisions; ++j) {
                contour_c(j, i) = interpolator.interpolate(
                    Eigen::Vector2d(contour_x(i), contour_y(j)));
            }
        }

        {
            // cspell: ignore zmin,zmax,zsmooth,colorbar
            auto fig = go.attr("Figure")(
                pybind11::arg("data") =
                    go.attr("Heatmap")(pybind11::arg("z") = contour_c,
                        pybind11::arg("x") = contour_x,
                        pybind11::arg("y") = contour_y,
                        pybind11::arg("zmin") = c_min,
                        pybind11::arg("zmax") = c_max,
                        pybind11::arg("zsmooth") = "best",
                        pybind11::arg("colorbar") =
                            pybind11::dict(pybind11::arg("title") = "value")));
            fig.attr("update_layout")(pybind11::arg("xaxis_title") = "x",
                pybind11::arg("yaxis_title") = "y");
            save(fig, "interpolated");
        }
    } catch (const std::exception& e) {
        fmt::print(stderr, "Exception thrown: {}\n", e.what());
        (void)std::fflush(stderr);
        return 1;
    }
}
