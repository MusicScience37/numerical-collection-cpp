/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Experiment to approximate a L-curve.
 */
#include <cmath>
#include <cstddef>
#include <random>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <plotly_plotter/data_table.h>
#include <plotly_plotter/figure_builders/line.h>
#include <plotly_plotter/write_html.h>
#include <plotly_plotter/write_png.h>

#include "num_collect/regularization/explicit_l_curve.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_prob_collect/regularization/blur_sine.h"

auto main() -> int {
    using coeff_type = Eigen::MatrixXd;
    using data_type = Eigen::VectorXd;
    static constexpr num_collect::index_type solution_size = 60;
    static constexpr num_collect::index_type data_size = 60;
    constexpr double error_rate = 0.01;

    // create test problem
    const auto prob =
        num_prob_collect::regularization::blur_sine(data_size, solution_size);
    std::mt19937 engine;  // NOLINT
    std::normal_distribution<double> dist{0.0,
        std::sqrt(prob.data().squaredNorm() /
            static_cast<double>(prob.data().size()) * error_rate)};
    auto data_with_error = prob.data();
    for (num_collect::index_type i = 0; i < data_with_error.size(); ++i) {
        data_with_error(i) += dist(engine);
    }

    // solve
    using solver_type =
        num_collect::regularization::tikhonov<coeff_type, data_type>;
    solver_type tikhonov;
    tikhonov.compute(prob.coeff(), data_with_error);

    using reference_searcher_type =
        num_collect::regularization::explicit_l_curve<solver_type>;
    reference_searcher_type reference_searcher{tikhonov};

    constexpr std::size_t num_samples = 21;
    std::vector<double> log_param_list;
    std::vector<double> log_residual_norm_list;
    std::vector<double> log_regularization_term_list;
    std::vector<double> curvature_list;
    const auto [min_param, max_param] = tikhonov.param_search_region();
    for (std::size_t i = 0; i < num_samples; ++i) {
        const double rate =
            static_cast<double>(i) / static_cast<double>(num_samples - 1);
        const double param = min_param * std::pow(max_param / min_param, rate);
        const double log_param = std::log10(param);
        log_param_list.push_back(log_param);

        const double residual_norm = tikhonov.residual_norm(param);
        const double log_residual_norm = std::log10(residual_norm);
        log_residual_norm_list.push_back(log_residual_norm);

        const double regularization_term = tikhonov.regularization_term(param);
        const double log_regularization_term = std::log10(regularization_term);
        log_regularization_term_list.push_back(log_regularization_term);

        const double curvature = tikhonov.l_curve_curvature(param);
        curvature_list.push_back(curvature);
    }
    std::vector<double> direction_list;
    std::vector<double> distance_list;
    for (std::size_t i = 0; i < num_samples - 1; ++i) {
        const double dx =
            log_residual_norm_list[i + 1] - log_residual_norm_list[i];
        const double dy = log_regularization_term_list[i + 1] -
            log_regularization_term_list[i];
        direction_list.push_back(std::atan2(dy, dx));
        distance_list.push_back(std::sqrt(dx * dx + dy * dy));
    }
    std::vector<double> approx_curvature_list;
    for (std::size_t i = 0; i < num_samples - 2; ++i) {
        const double direction_diff = direction_list[i + 1] - direction_list[i];
        const double mean_distance =
            0.5 * (distance_list[i + 1] + distance_list[i]);
        approx_curvature_list.push_back(direction_diff / mean_distance);
    }

    {
        std::vector<double> plot_param_list;
        std::vector<std::string> plot_type_list;
        std::vector<double> plot_value_list;
        for (std::size_t i = 0; i < log_param_list.size(); ++i) {
            // NOLINTNEXTLINE
            plot_param_list.push_back(std::pow(10.0, log_param_list[i]));
            plot_type_list.emplace_back("Log Residual Norm");
            plot_value_list.push_back(log_residual_norm_list[i]);

            // NOLINTNEXTLINE
            plot_param_list.push_back(std::pow(10.0, log_param_list[i]));
            plot_type_list.emplace_back("Log Regularization Term");
            plot_value_list.push_back(log_regularization_term_list[i]);
        }
        for (std::size_t i = 0; i < log_param_list.size() - 1; ++i) {
            plot_param_list.push_back(std::pow(
                // NOLINTNEXTLINE
                10.0, 0.5 * (log_param_list[i] + log_param_list[i + 1])));
            plot_type_list.emplace_back("Direction");
            plot_value_list.push_back(direction_list[i]);
        }
        for (std::size_t i = 0; i < log_param_list.size() - 2; ++i) {
            // NOLINTNEXTLINE
            plot_param_list.push_back(std::pow(10.0, log_param_list[i + 1]));
            plot_type_list.emplace_back("Approximate Curvature");
            plot_value_list.push_back(approx_curvature_list[i]);
        }
        for (std::size_t i = 0; i < log_param_list.size(); ++i) {
            // NOLINTNEXTLINE
            plot_param_list.push_back(std::pow(10.0, log_param_list[i]));
            plot_type_list.emplace_back("Curvature");
            plot_value_list.push_back(curvature_list[i]);
        }

        plotly_plotter::data_table data;
        data.emplace("Regularization Parameter", plot_param_list);
        data.emplace("Value Type", plot_type_list);
        data.emplace("Value", plot_value_list);

        const auto fig =
            plotly_plotter::figure_builders::line(data)
                .x("Regularization Parameter")
                .y("Value")
                .subplot_row("Value Type")
                .log_x(true)
                .title("Values of L-curve in Tikhonov Regularization")
                .create();

        plotly_plotter::write_html("reg_approx_l_curve.html", fig);
        if (plotly_plotter::is_png_supported()) {
            // NOLINTNEXTLINE
            plotly_plotter::write_png("reg_approx_l_curve.png", fig, 800, 1600);
        }
    }
}
