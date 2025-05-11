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
 * \brief Example to plot GCV calculated for FISTA.
 */
#include <cmath>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <plotly_plotter/data_table.h>
#include <plotly_plotter/figure_builders/line.h>
#include <plotly_plotter/write_html.h>
#include <plotly_plotter/write_png.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"
#include "num_collect/regularization/fista.h"

static void add_circle(
    Eigen::MatrixXd& image, const Eigen::Vector2d& center, double radius) {
    const num_collect::index_type rows = image.rows();
    const num_collect::index_type cols = image.cols();
    Eigen::Vector2d point;
    for (num_collect::index_type j = 0; j < cols; ++j) {
        const double x = static_cast<double>(j) / static_cast<double>(cols - 1);
        point.x() = x;
        for (num_collect::index_type i = 0; i < rows; ++i) {
            const double y =
                static_cast<double>(i) / static_cast<double>(rows - 1);
            point.y() = y;
            const double dist = (point - center).norm();
            if (dist <= radius) {
                image(i, j) = 1.0;
            }
        }
    }
}

static void add_noise(Eigen::MatrixXd& image, double rate) {
    std::mt19937 engine;  // NOLINT
    const double sigma = std::sqrt(
        image.squaredNorm() / static_cast<double>(image.size()) * rate);
    std::normal_distribution<double> dist{0.0, sigma};

    const num_collect::index_type rows = image.rows();
    const num_collect::index_type cols = image.cols();
    for (num_collect::index_type j = 0; j < cols; ++j) {
        for (num_collect::index_type i = 0; i < rows; ++i) {
            image(i, j) += dist(engine);
        }
    }
}

auto main() -> int {
    num_collect::logging::logger logger;

#ifndef NDEBUG
    constexpr num_collect::index_type rows = 20;
    constexpr num_collect::index_type cols = 20;
    constexpr double noise_rate = 0.05;
#else
    constexpr num_collect::index_type rows = 40;
    constexpr num_collect::index_type cols = 40;
    constexpr double noise_rate = 0.1;
#endif
    constexpr num_collect::index_type size = rows * cols;

    const Eigen::Vector2d center = Eigen::Vector2d(0.7, 0.6);
    constexpr double radius = 0.2;
    Eigen::MatrixXd origin = Eigen::MatrixXd::Zero(rows, cols);
    add_circle(origin, center, radius);

    Eigen::MatrixXd data = origin;
    add_noise(data, noise_rate);

    using coeff_type = Eigen::SparseMatrix<double>;
    coeff_type coeff;
    coeff.resize(size, size);
    coeff.setIdentity();

    using solver_type =
        num_collect::regularization::fista<coeff_type, Eigen::VectorXd>;
    solver_type solver;
    const Eigen::VectorXd data_vec = data.reshaped();
    solver.compute(coeff, data_vec);

    const auto [param_lower_bound, param_upper_bound] =
        solver.param_search_region();
    logger.info()(
        "param_search_region: [{}, {}]", param_lower_bound, param_upper_bound);

    std::vector<double> param_list;
    std::vector<double> residual_norm_list;
    std::vector<double> regularization_term_list;
    constexpr std::size_t num_samples = 21;
    for (std::size_t i = 0; i < num_samples; ++i) {
        const double rate =
            static_cast<double>(i) / static_cast<double>(num_samples - 1);
        const double param = param_lower_bound *
            std::pow(param_upper_bound / param_lower_bound, rate);

        Eigen::VectorXd solution_vec = data_vec;
        solver.solve(param, solution_vec);
        const double residual_norm = solver.residual_norm(solution_vec);
        const double regularization_term =
            solver.regularization_term(solution_vec);

        param_list.push_back(param);
        residual_norm_list.push_back(residual_norm);
        regularization_term_list.push_back(regularization_term);
    }

    plotly_plotter::data_table plot_data;
    plot_data.emplace("Regularization Parameter", param_list);
    plot_data.emplace("Residual Norm", residual_norm_list);
    plot_data.emplace("Regularization Term", regularization_term_list);

    auto fig = plotly_plotter::figure_builders::line(plot_data)
                   .x("Residual Norm")
                   .y("Regularization Term")
                   .title("L-curve")
                   .log_x(true)
                   .log_y(true)
                   .hover_data({"Regularization Parameter"})
                   .create();

    plotly_plotter::write_html("plot_l_curve_with_fista.html", fig);
    plotly_plotter::write_png("plot_l_curve_with_fista.png", fig);
}
