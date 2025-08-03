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
 * \brief Definition of common functions for upsampling.
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

struct upsampling_config {
    num_collect::index_type original_rows;
    num_collect::index_type original_cols;
    num_collect::index_type upsampled_rows;
    num_collect::index_type upsampled_cols;
    int sample_image_index;
};

/*!
 * \brief Perform common initialization for upsampling examples and
 * generate a matrix of the original image.
 *
 * \param[in] argc Number of command line arguments.
 * \param[in] argv Command line arguments.
 * \return Configuration for upsampling or `std::nullopt` if failed.
 */
[[nodiscard]] inline auto initialize(int argc, char** argv)
    -> std::optional<upsampling_config> {
#ifndef NDEBUG
    std::string config_file_path =
        "examples/regularization/upsampling_config_small.toml";
#else
    std::string config_file_path =
        "examples/regularization/upsampling_config.toml";
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
    upsampling_config config;
    try {
        config.original_rows = config_table.at_path("upsampling.original_rows")
                                   .value<num_collect::index_type>()
                                   .value();
        config.original_cols = config_table.at_path("upsampling.original_cols")
                                   .value<num_collect::index_type>()
                                   .value();
        config.upsampled_rows =
            config_table.at_path("upsampling.upsampled_rows")
                .value<num_collect::index_type>()
                .value();
        config.upsampled_cols =
            config_table.at_path("upsampling.upsampled_cols")
                .value<num_collect::index_type>()
                .value();
        config.sample_image_index =
            config_table.at_path("upsampling.sample_image_index")
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
    const upsampling_config& config, Eigen::MatrixXd& origin) -> bool {
    switch (config.sample_image_index) {
    case 1:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_constant_circle(
                origin, config.original_rows, config.original_cols);
        break;
    case 2:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_two_constant_circles(
                origin, config.original_rows, config.original_cols);
        break;
    case 3:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_quadratic_circle(
                origin, config.original_rows, config.original_cols);
        break;
    case 4:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_smooth_circle(
                origin, config.original_rows, config.original_cols);
        break;
    case 5:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_two_smooth_circles(
                origin, config.original_rows, config.original_cols);
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
 * \param[in] upsampled Upsampled image.
 * \param[in] algorithm_name_for_title Name of the algorithm used for solving,
 * for title of the plot.
 * \param[in] algorithm_name_for_file_name Name of the algorithm used for
 * solving, for file name of the plot.
 */
inline void visualize_result(const Eigen::MatrixXd& origin,
    const Eigen::MatrixXd& upsampled,
    const std::string& algorithm_name_for_title,
    const std::string& algorithm_name_for_file_name) {
    num_collect::logging::logger logger;

    plotly_plotter::figure figure;

    figure.layout().grid().rows(1);
    figure.layout().grid().columns(2);
    figure.layout().grid().pattern("independent");

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
    heatmap.z(upsampled);
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
    annotation.text("Upsampled Image");

    figure.layout().yaxis().scale_anchor("x");
    figure.layout().yaxis(2).scale_anchor("x2");

    figure.layout().color_axis().color_scale(
        plotly_plotter::color_scales::autumn_full());
    figure.layout().color_axis().c_min(0.0);
    figure.layout().color_axis().c_max(1.0);

    figure.title(fmt::format("Upsampling with {}", algorithm_name_for_title));

    const auto file_path =
        fmt::format("./upsampling_{}.html", algorithm_name_for_file_name);
    plotly_plotter::write_html(file_path, figure);
    NUM_COLLECT_LOG_INFO(logger, "Wrote a plot to {}", file_path);
}
