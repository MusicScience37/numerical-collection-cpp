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
 * \brief Example of RBF interpolation in 2D.
 */
#include <cstdio>
#include <string_view>
#include <vector>

#include <Eigen/Core>
#include <fmt/format.h>
#include <plotly_plotter/color_scales.h>
#include <plotly_plotter/data_table.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/figure_builders/scatter.h>
#include <plotly_plotter/write_html.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/util/vector.h"

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
    const num_collect::util::vector<Eigen::Vector2d>& variables,
    const Eigen::VectorXd& values) {
    plotly_plotter::data_table data;
    auto x = data.emplace<double>("x");
    auto y = data.emplace<double>("y");
    auto value = data.emplace<double>("value");
    for (num_collect::index_type i = 0; i < num_sample_points; ++i) {
        x->push_back(variables[i].x());
        y->push_back(variables[i].y());
        value->push_back(values(i));
    }
    return data;
}

static void save(const plotly_plotter::figure& fig, std::string_view name) {
    plotly_plotter::write_html(fmt::format("rbf_{}.html", name), fig);
}

auto main() -> int {
    auto sample_variables =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_sample_points);
    for (auto& variable : sample_variables) {
        variable.x() = x_min + (x_max - x_min) * variable.x();
        variable.y() = y_min + (y_max - y_min) * variable.y();
    }
    Eigen::VectorXd sample_values = Eigen::VectorXd::Zero(num_sample_points);
    for (num_collect::index_type i = 0; i < num_sample_points; ++i) {
        sample_values(i) = target_function(sample_variables[i]);
    }

    {
        auto fig = plotly_plotter::figure_builders::scatter(
            convert_to_plotly_data(sample_variables, sample_values))
                       .x("x")
                       .y("y")
                       .marker_color("value")
                       .create();
        fig.layout().color_axis().c_min(c_min);
        fig.layout().color_axis().c_max(c_max);
        fig.layout().color_axis().color_scale(
            plotly_plotter::color_scales::autumn());
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
        plotly_plotter::figure fig;

        auto heatmap = fig.add_heatmap();
        heatmap.z(contour_c);
        heatmap.x(contour_x);
        heatmap.y(contour_y);
        heatmap.color_axis("coloraxis");
        heatmap.z_smooth("best");

        fig.x_title("x");
        fig.y_title("y");
        fig.layout().color_axis().c_min(c_min);
        fig.layout().color_axis().c_max(c_max);
        fig.layout().color_axis().color_bar().title().text("value");
        fig.layout().color_axis().color_scale(
            plotly_plotter::color_scales::autumn());

        save(fig, "interpolated");
    }
}
