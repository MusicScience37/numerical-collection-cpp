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
 * \brief Definition of common functions for image denoising.
 */
#pragma once

#include <iostream>
#include <optional>
#include <string>

#include <Eigen/Core>
#include <fmt/format.h>
#include <lyra/lyra.hpp>
#include <plotly_plotter/color_scales.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>
#include <toml++/toml.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_prob_collect/regularization/generate_sparse_sample_image.h"

struct image_denoising_config {
    num_collect::index_type rows;
    num_collect::index_type cols;
    double noise_rate;
    int sample_image_index;
};

/*!
 * \brief Perform common initialization for image denoising examples and
 * generate a matrix of the original image.
 *
 * \param[in] argc Number of command line arguments.
 * \param[in] argv Command line arguments.
 * \return Configuration for image denoising or `std::nullopt` if failed.
 */
[[nodiscard]] inline auto initialize(int argc, char** argv)
    -> std::optional<image_denoising_config> {
#ifndef NDEBUG
    std::string config_file_path =
        "examples/regularization/image_denoising_config_small.toml";
#else
    std::string config_file_path =
        "examples/regularization/image_denoising_config.toml";
#endif
    const auto cli =
        lyra::cli().add_argument(lyra::arg(config_file_path, "config_file_path")
                .optional()
                .help("Path to the configuration file."));
    const auto result = cli.parse({argc, argv});
    if (!result) {
        std::cerr << result.message() << "\n\n";
        std::cerr << cli << std::endl;
        return std::nullopt;
    }

    num_collect::logging::load_logging_config_file(config_file_path);

    const auto config_table = toml::parse_file(config_file_path);
    image_denoising_config config;
    try {
        config.rows = config_table.at_path("image_denoising.rows")
                          .value<num_collect::index_type>()
                          .value();
        config.cols = config_table.at_path("image_denoising.cols")
                          .value<num_collect::index_type>()
                          .value();
        config.noise_rate = config_table.at_path("image_denoising.noise_rate")
                                .value<double>()
                                .value();
        config.sample_image_index =
            config_table.at_path("image_denoising.sample_image_index")
                .value<int>()
                .value();
    } catch (const std::exception& /*exception*/) {
        std::cerr << "Invalid configuration file: " << config_file_path
                  << std::endl;
        return std::nullopt;
    }

    return config;
}

/*!
 * \brief Generate a sample image matrix.
 *
 * \param[in] config Configuration for image denoising.
 * \param[out] origin Original image matrix.
 * \retval true Sample image generated successfully.
 * \retval false Failed to generate the sample image.
 */
[[nodiscard]] inline auto generate_sample_image(
    const image_denoising_config& config, Eigen::MatrixXd& origin) -> bool {
    switch (config.sample_image_index) {
    case 1:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_constant_circle(
                origin, config.rows, config.cols);
        break;
    case 2:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_two_constant_circles(
                origin, config.rows, config.cols);
        break;
    case 3:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_quadratic_circle(
                origin, config.rows, config.cols);
        break;
    case 4:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_smooth_circle(
                origin, config.rows, config.cols);
        break;
    case 5:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_two_smooth_circles(
                origin, config.rows, config.cols);
        break;
    default:
        std::cerr << "Unknown sample image index: " << config.sample_image_index
                  << std::endl;
        return false;
    }
    return true;
}

/*!
 * \brief Visualize the result using heatmaps.
 *
 * \param[in] origin Original image.
 * \param[in] data Data image with noise.
 * \param[in] solution Solution image.
 * \param[in] algorithm_name_for_title Name of the algorithm used for solving,
 * for title of the plot.
 * \param[in] algorithm_name_for_file_name Name of the algorithm used for
 * solving, for file name of the plot.
 */
inline void visualize_result(const Eigen::MatrixXd& origin,
    const Eigen::MatrixXd& data, const Eigen::MatrixXd& solution,
    const std::string& algorithm_name_for_title,
    const std::string& algorithm_name_for_file_name) {
    num_collect::logging::logger logger;

    const Eigen::MatrixXd error = (solution - origin).cwiseAbs();
    const double error_rate = error.squaredNorm() / origin.squaredNorm();
    const double actual_noise_rate =
        (data - origin).squaredNorm() / origin.squaredNorm();
    NUM_COLLECT_LOG_INFO(logger, "Noise rate: {}", actual_noise_rate);
    NUM_COLLECT_LOG_INFO(logger, "Error rate: {}", error_rate);

    plotly_plotter::figure figure;

    figure.layout().grid().rows(2);
    figure.layout().grid().columns(2);
    figure.layout().grid().pattern("coupled");

    constexpr double annotation_x = 0.5;
    constexpr double annotation_y = 1.0;
    constexpr double annotation_y_shift = 30.0;

    auto heatmap = figure.add_heatmap();
    heatmap.z(origin);
    heatmap.xaxis("x");
    heatmap.yaxis("y");
    heatmap.color_axis("coloraxis");
    auto annotation = figure.layout().add_annotation();
    annotation.x_ref("x domain");
    annotation.y_ref("y domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("Original Image");

    heatmap = figure.add_heatmap();
    heatmap.z(data);
    heatmap.xaxis("x2");
    heatmap.yaxis("y");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x2 domain");
    annotation.y_ref("y domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("Data Image with Noise");

    heatmap = figure.add_heatmap();
    heatmap.z(solution);
    heatmap.xaxis("x");
    heatmap.yaxis("y2");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x domain");
    annotation.y_ref("y2 domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("Solution Image");

    heatmap = figure.add_heatmap();
    heatmap.z(error);
    heatmap.xaxis("x2");
    heatmap.yaxis("y2");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x2 domain");
    annotation.y_ref("y2 domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("Error");

    figure.layout().xaxis(2).matches("x");
    figure.layout().yaxis().scale_anchor("x");
    figure.layout().yaxis(2).matches("y");

    figure.layout().color_axis().color_scale(
        plotly_plotter::color_scales::autumn_full());
    figure.layout().color_axis().c_min(0.0);
    figure.layout().color_axis().c_max(1.0);

    figure.title(
        fmt::format("Image Denoising with {}", algorithm_name_for_title));

    const auto file_path = fmt::format(
        "./sparse_image_regularization_{}.html", algorithm_name_for_file_name);
    plotly_plotter::write_html(file_path, figure);
    NUM_COLLECT_LOG_INFO(logger, "Wrote a plot to {}", file_path);
}
