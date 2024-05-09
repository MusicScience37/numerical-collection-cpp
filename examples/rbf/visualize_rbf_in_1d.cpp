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
 * \brief Example to visualize RBFs used in interpolation in 1D.
 */
#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <fmt/core.h>
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/rbf_interpolator.h"

static constexpr double x_max = 10.0;
static constexpr double y_max = 5.0;
static constexpr double y_min = 0.0;
static constexpr std::size_t line_points = 101;

using interpolator_type =
    num_collect::rbf::global_rbf_interpolator<double(double)>;

static void plot_sample_points(const pybind11::object& fig,
    const pybind11::module& go, const std::vector<double>& sample_point_x_list,
    const Eigen::VectorXd& sample_point_y_list) {
    fig.attr("add_trace")(
        go.attr("Scatter")(pybind11::arg("x") = sample_point_x_list,
            pybind11::arg("y") = sample_point_y_list,
            pybind11::arg("mode") = "markers",
            pybind11::arg("marker") =
                pybind11::dict(pybind11::arg("color") = "#66F",
                    // NOLINTNEXTLINE
                    pybind11::arg("size") = 10)));
}

static void plot_rbfs_without_coeffs(const pybind11::object& fig,
    const pybind11::module& go, const interpolator_type& interpolator,
    const std::vector<double>& sample_point_x_list) {
    const double length_parameter =
        interpolator.length_parameter_calculator().length_parameter_at(0);
    std::vector<double> x_list;
    std::vector<std::vector<double>> y_list_list;
    x_list.reserve(line_points);
    y_list_list.resize(sample_point_x_list.size());
    for (auto& y_list : y_list_list) {
        y_list.reserve(line_points);
    }
    for (std::size_t i = 0; i < line_points; ++i) {
        const double x = x_max * static_cast<double>(i) /
            static_cast<double>(line_points - 1U);
        x_list.push_back(x);
        for (std::size_t j = 0; j < sample_point_x_list.size(); ++j) {
            const double distance =
                interpolator.distance_function()(x, sample_point_x_list[j]);
            const double y = interpolator.rbf()(distance / length_parameter);
            y_list_list[j].push_back(y);
        }
    }
    for (const auto& y_list : y_list_list) {
        fig.attr("add_trace")(go.attr("Scatter")(pybind11::arg("x") = x_list,
            pybind11::arg("y") = y_list, pybind11::arg("mode") = "lines",
            pybind11::arg("line") =
                pybind11::dict(pybind11::arg("color") = "#2A2")));
    }
}

static void plot_rbfs_with_coeffs(const pybind11::object& fig,
    const pybind11::module& go, const interpolator_type& interpolator,
    const std::vector<double>& sample_point_x_list) {
    const double length_parameter =
        interpolator.length_parameter_calculator().length_parameter_at(0);
    std::vector<double> x_list;
    std::vector<std::vector<double>> y_list_list;
    x_list.reserve(line_points);
    y_list_list.resize(sample_point_x_list.size());
    for (auto& y_list : y_list_list) {
        y_list.reserve(line_points);
    }
    for (std::size_t i = 0; i < line_points; ++i) {
        const double x = x_max * static_cast<double>(i) /
            static_cast<double>(line_points - 1U);
        x_list.push_back(x);
        for (std::size_t j = 0; j < sample_point_x_list.size(); ++j) {
            const double distance =
                interpolator.distance_function()(x, sample_point_x_list[j]);
            const double y =
                interpolator.coeffs()(static_cast<num_collect::index_type>(j)) *
                interpolator.rbf()(distance / length_parameter);
            y_list_list[j].push_back(y);
        }
    }
    for (const auto& y_list : y_list_list) {
        fig.attr("add_trace")(go.attr("Scatter")(pybind11::arg("x") = x_list,
            pybind11::arg("y") = y_list, pybind11::arg("mode") = "lines",
            pybind11::arg("line") =
                pybind11::dict(pybind11::arg("color") = "#2A2")));
    }
}

static void plot_interpolated_curve(const pybind11::object& fig,
    const pybind11::module& go, const interpolator_type& interpolator) {
    std::vector<double> x_list;
    std::vector<double> y_list;
    x_list.reserve(line_points);
    y_list.reserve(line_points);
    for (std::size_t i = 0; i < line_points; ++i) {
        const double x = x_max * static_cast<double>(i) /
            static_cast<double>(line_points - 1U);
        const double y = interpolator.interpolate(x);
        x_list.push_back(x);
        y_list.push_back(y);
    }
    fig.attr("add_trace")(go.attr("Scatter")(pybind11::arg("x") = x_list,
        pybind11::arg("y") = y_list, pybind11::arg("mode") = "lines",
        pybind11::arg("line") =
            pybind11::dict(pybind11::arg("color") = "#E53")));
}

static void set_layout(const pybind11::object& fig) {
    // cspell: ignore showlegend,xaxes,yaxes,showticklabels,showline,linecolor,linewidth
    fig.attr("update_layout")(pybind11::arg("xaxis_title") = "x",
        pybind11::arg("yaxis_title") = "y",
        pybind11::arg("xaxis_range") = std::make_pair(0.0, x_max),
        pybind11::arg("yaxis_range") = std::make_pair(y_min, y_max),
        pybind11::arg("plot_bgcolor") = "white",
        pybind11::arg("showlegend") = false);
    fig.attr("update_xaxes")(pybind11::arg("showticklabels") = false,
        pybind11::arg("showline") = true, pybind11::arg("linecolor") = "black",
        pybind11::arg("linewidth") = 2);
    fig.attr("update_yaxes")(pybind11::arg("showticklabels") = false,
        pybind11::arg("showline") = true, pybind11::arg("linecolor") = "black",
        pybind11::arg("linewidth") = 2);
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

        const auto sample_point_x_list = std::vector<double>{1.0, 4.0, 9.0};
        const auto sample_point_y_list = Eigen::VectorXd{{3.0, 4.0, 2.0}};

        {
            auto fig = go.attr("Figure")();
            plot_sample_points(
                fig, go, sample_point_x_list, sample_point_y_list);
            set_layout(fig);
            save(fig, "sample_points");
        }

        interpolator_type interpolator;
        constexpr double length_parameter_scale = 0.7;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_point_x_list, sample_point_y_list);

        {
            auto fig = go.attr("Figure")();
            plot_interpolated_curve(fig, go, interpolator);
            plot_sample_points(
                fig, go, sample_point_x_list, sample_point_y_list);
            set_layout(fig);
            save(fig, "interpolated_curve");
        }
        {
            auto fig = go.attr("Figure")();
            plot_rbfs_with_coeffs(fig, go, interpolator, sample_point_x_list);
            plot_interpolated_curve(fig, go, interpolator);
            plot_sample_points(
                fig, go, sample_point_x_list, sample_point_y_list);
            set_layout(fig);
            save(fig, "interpolated_curve_with_used_rbfs");
        }
        {
            auto fig = go.attr("Figure")();
            plot_rbfs_with_coeffs(fig, go, interpolator, sample_point_x_list);
            plot_sample_points(
                fig, go, sample_point_x_list, sample_point_y_list);
            set_layout(fig);
            save(fig, "used_rbfs");
        }
        {
            auto fig = go.attr("Figure")();
            plot_rbfs_without_coeffs(
                fig, go, interpolator, sample_point_x_list);
            plot_sample_points(
                fig, go, sample_point_x_list, sample_point_y_list);
            set_layout(fig);
            save(fig, "plain_rbfs");
        }
    } catch (const std::exception& e) {
        fmt::print(stderr, "Exception thrown: {}\n", e.what());
        (void)std::fflush(stderr);
        return 1;
    }
}
