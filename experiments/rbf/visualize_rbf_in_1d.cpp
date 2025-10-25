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
#include <string_view>
#include <vector>

#include <Eigen/Core>
#include <fmt/format.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>
#include <plotly_plotter/write_png.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/rbf_interpolator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

static constexpr double x_max = 10.0;
static constexpr double y_max = 5.0;
static constexpr double y_min = 0.0;
static constexpr std::size_t line_points = 101;

using interpolator_type =
    num_collect::rbf::global_rbf_interpolator<double(double)>;

static void plot_sample_points(plotly_plotter::figure& fig,
    const std::vector<double>& sample_point_x_list,
    const Eigen::VectorXd& sample_point_y_list) {
    auto trace = fig.add_scatter();
    trace.x(sample_point_x_list);
    trace.y(sample_point_y_list);
    trace.mode("markers");
    trace.marker().color("#66F");
    trace.marker().size(10);
}

static void plot_rbfs_without_coeffs(plotly_plotter::figure& fig,
    const interpolator_type& interpolator,
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
        auto trace = fig.add_scatter();
        trace.x(x_list);
        trace.y(y_list);
        trace.mode("lines");
        trace.color("#2A2");
    }
}

static void plot_rbfs_with_coeffs(plotly_plotter::figure& fig,
    const interpolator_type& interpolator,
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
        auto trace = fig.add_scatter();
        trace.x(x_list);
        trace.y(y_list);
        trace.mode("lines");
        trace.color("#2A2");
    }
}

static void plot_interpolated_curve(
    plotly_plotter::figure& fig, const interpolator_type& interpolator) {
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
    auto trace = fig.add_scatter();
    trace.x(x_list);
    trace.y(y_list);
    trace.mode("lines");
    trace.color("#E53");
}

static void set_layout(plotly_plotter::figure& fig) {
    fig.x_title("x");
    fig.y_title("y");
    fig.layout().xaxis().range(0.0, x_max);
    fig.layout().yaxis().range(y_min, y_max);
    fig.layout().show_legend(false);
    fig.layout().xaxis().show_tick_labels(false);
    fig.layout().xaxis().show_line(true);
    fig.layout().xaxis().line_color("black");
    fig.layout().xaxis().line_width(2.0);
    fig.layout().yaxis().show_tick_labels(false);
    fig.layout().yaxis().show_line(true);
    fig.layout().yaxis().line_color("black");
    fig.layout().yaxis().line_width(2.0);
    fig.layout().plot_bg_color("#FFFFFF");
}

static void save(plotly_plotter::figure& fig, std::string_view name) {
    plotly_plotter::write_html(fmt::format("rbf_{}.html", name), fig);
    if (plotly_plotter::is_png_supported()) {
        plotly_plotter::write_png(fmt::format("rbf_{}.png", name), fig);
    }
}

auto main() -> int {
    const auto sample_point_x_list = std::vector<double>{1.0, 4.0, 9.0};
    const auto sample_point_y_list = Eigen::VectorXd{{3.0, 4.0, 2.0}};

    {
        plotly_plotter::figure fig;
        plot_sample_points(fig, sample_point_x_list, sample_point_y_list);
        set_layout(fig);
        save(fig, "sample_points");
    }

    interpolator_type interpolator;
    constexpr double length_parameter_scale = 0.7;
    interpolator.fix_length_parameter_scale(length_parameter_scale);
    interpolator.compute(sample_point_x_list, sample_point_y_list);

    {
        plotly_plotter::figure fig;
        plot_interpolated_curve(fig, interpolator);
        plot_sample_points(fig, sample_point_x_list, sample_point_y_list);
        set_layout(fig);
        save(fig, "interpolated_curve");
    }
    {
        plotly_plotter::figure fig;
        plot_rbfs_with_coeffs(fig, interpolator, sample_point_x_list);
        plot_interpolated_curve(fig, interpolator);
        plot_sample_points(fig, sample_point_x_list, sample_point_y_list);
        set_layout(fig);
        save(fig, "interpolated_curve_with_used_rbfs");
    }
    {
        plotly_plotter::figure fig;
        plot_rbfs_with_coeffs(fig, interpolator, sample_point_x_list);
        plot_sample_points(fig, sample_point_x_list, sample_point_y_list);
        set_layout(fig);
        save(fig, "used_rbfs");
    }
    {
        plotly_plotter::figure fig;
        plot_rbfs_without_coeffs(fig, interpolator, sample_point_x_list);
        plot_sample_points(fig, sample_point_x_list, sample_point_y_list);
        set_layout(fig);
        save(fig, "plain_rbfs");
    }
}
