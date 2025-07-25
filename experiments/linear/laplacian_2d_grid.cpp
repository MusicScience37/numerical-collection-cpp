/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Example to solve Laplace equation on a 2-dimensional grid.
 */
#include "num_prob_collect/linear/laplacian_2d_grid.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>
#include <plotly_plotter/color_scales.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>
#include <plotly_plotter/write_png.h>
#include <toml++/toml.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"

using mat_type = Eigen::SparseMatrix<double>;
using vec_type = Eigen::VectorXd;
using grid_type = num_prob_collect::linear::laplacian_2d_grid<mat_type>;

constexpr double region_size = 1.0;

static auto expected_function(double x, double y) { return x * x + y * y; }

static auto make_right_vec(
    num_collect::index_type grid_size, const grid_type& grid) {
    vec_type right_vec =
        vec_type::Constant((grid_size - 1) * (grid_size - 1), -4.0);  // NOLINT

    const double off_diag_coeff = grid.off_diag_coeff();

    // x = 0
    {
        const double x = 0.0;
        const num_collect::index_type xi = 0;
        for (num_collect::index_type yj = -1, yj_end = grid_size; yj < yj_end;
            ++yj) {
            const double y =
                static_cast<double>(yj + 1) / static_cast<double>(grid_size);
            const double val = expected_function(x, y);
            for (num_collect::index_type
                     yi = std::max<num_collect::index_type>(yj - 1, 0),
                     yi_end = std::min<num_collect::index_type>(
                         yj + 2, grid_size - 1);
                yi < yi_end; ++yi) {
                const num_collect::index_type i = grid.index(xi, yi);
                right_vec(i) -= off_diag_coeff * val;
            }
        }
    }
    // x = 1
    {
        const double x = 1.0;
        const num_collect::index_type xi = grid_size - 2;
        for (num_collect::index_type yj = -1, yj_end = grid_size; yj < yj_end;
            ++yj) {
            const double y =
                static_cast<double>(yj + 1) / static_cast<double>(grid_size);
            const double val = expected_function(x, y);
            for (num_collect::index_type
                     yi = std::max<num_collect::index_type>(yj - 1, 0),
                     yi_end = std::min<num_collect::index_type>(
                         yj + 2, grid_size - 1);
                yi < yi_end; ++yi) {
                const num_collect::index_type i = grid.index(xi, yi);
                right_vec(i) -= off_diag_coeff * val;
            }
        }
    }
    // y = 0
    {
        const double y = 0.0;
        const num_collect::index_type yi = 0;
        for (num_collect::index_type xj = 0, xj_end = grid_size - 1;
            xj < xj_end; ++xj) {
            const double x =
                static_cast<double>(xj + 1) / static_cast<double>(grid_size);
            const double val = expected_function(x, y);
            for (num_collect::index_type
                     xi = std::max<num_collect::index_type>(xj - 1, 0),
                     xi_end = std::min<num_collect::index_type>(
                         xj + 2, grid_size - 1);
                xi < xi_end; ++xi) {
                const num_collect::index_type i = grid.index(xi, yi);
                right_vec(i) -= off_diag_coeff * val;
            }
        }
    }
    // y = 1
    {
        const double y = 1.0;
        const num_collect::index_type yi = grid_size - 2;
        for (num_collect::index_type xj = 0, xj_end = grid_size - 1;
            xj < xj_end; ++xj) {
            const double x =
                static_cast<double>(xj + 1) / static_cast<double>(grid_size);
            const double val = expected_function(x, y);
            for (num_collect::index_type
                     xi = std::max<num_collect::index_type>(xj - 1, 0),
                     xi_end = std::min<num_collect::index_type>(
                         xj + 2, grid_size - 1);
                xi < xi_end; ++xi) {
                const num_collect::index_type i = grid.index(xi, yi);
                right_vec(i) -= off_diag_coeff * val;
            }
        }
    }

    return right_vec;
}

static auto plot_result(const vec_type& expected, const vec_type& actual,
    num_collect::index_type grid_size) {
    plotly_plotter::figure figure;

    const Eigen::MatrixXd expected_mat =
        expected.reshaped(grid_size - 1, grid_size - 1);
    const Eigen::MatrixXd actual_mat =
        actual.reshaped(grid_size - 1, grid_size - 1);

    auto heatmap = figure.add_heatmap();
    heatmap.z(expected_mat);
    heatmap.xaxis("x");
    heatmap.yaxis("y");
    heatmap.color_axis("coloraxis");

    heatmap = figure.add_heatmap();
    heatmap.z(actual_mat);
    heatmap.xaxis("x2");
    heatmap.yaxis("y");
    heatmap.color_axis("coloraxis");

    figure.layout().grid().rows(1);
    figure.layout().grid().columns(2);
    figure.layout().grid().pattern("coupled");

    figure.layout().yaxis(2).matches("y");
    figure.layout().color_axis().color_scale(
        plotly_plotter::color_scales::autumn());

    plotly_plotter::write_html("laplacian_2d_grid.html", figure);
    if (plotly_plotter::is_png_supported()) {
        plotly_plotter::write_png("laplacian_2d_grid.png", figure);
    }
}

auto main(int argc, char** argv) -> int {
    std::string_view config_filepath =
        "experiments/linear/laplacian_2d_grid.toml";
    if (argc == 2) {
        config_filepath = argv[1];  // NOLINT
    }
    num_collect::logging::load_logging_config_file(
        std::string(config_filepath));
    num_collect::logging::logger logger;

    const auto config_table = toml::parse_file(config_filepath);
    const auto grid_size = config_table.at_path("laplacian_2d_grid.grid_size")
                               .value<num_collect::index_type>()
                               .value();
    logger.info()("Grid size: {} x {}", grid_size, grid_size);

    const double grid_width = region_size / static_cast<double>(grid_size);
    logger.info()("Grid width: {}", grid_width);

    grid_type grid{grid_size - 1, grid_size - 1, grid_width};
    logger.info()("Generated grid.");

    vec_type expected_sol(grid.mat_size());
    for (num_collect::index_type xi = 0; xi < grid_size - 1; ++xi) {
        const double x =
            static_cast<double>(xi + 1) / static_cast<double>(grid_size);
        for (num_collect::index_type yi = 0; yi < grid_size - 1; ++yi) {
            const double y =
                static_cast<double>(yi + 1) / static_cast<double>(grid_size);
            const num_collect::index_type i = grid.index(xi, yi);
            expected_sol(i) = expected_function(x, y);
        }
    }

    const vec_type right_vec = make_right_vec(grid_size, grid);

    logger.info()("Start preparation.");
    Eigen::ConjugateGradient<mat_type, Eigen::Upper | Eigen::Lower> solver;
    solver.compute(grid.mat());
    logger.info()("Start to solve.");
    const vec_type sol = solver.solve(right_vec);
    logger.info()("Finished to solve.");

    const double max_err = (sol - expected_sol).cwiseAbs().maxCoeff();
    logger.info()("Maximum error: {}", max_err);

    plot_result(expected_sol, sol, grid_size);

    return 0;
}
