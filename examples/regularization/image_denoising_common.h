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
#include <string>

#include <Eigen/Core>
#include <fmt/format.h>
#include <lyra/lyra.hpp>
#include <plotly_plotter/color_scales.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/opt/gaussian_process_optimizer.h"
#include "num_prob_collect/regularization/generate_sparse_sample_image.h"

#ifndef NDEBUG
constexpr num_collect::index_type rows = 20;
constexpr num_collect::index_type cols = 20;
constexpr double noise_rate = 0.05;
#else
constexpr num_collect::index_type rows = 40;
constexpr num_collect::index_type cols = 40;
constexpr double noise_rate = 0.1;
#endif

/*!
 * \brief Perform common initialization for image denoising examples and
 * generate a matrix of the original image.
 *
 * \param[in] argc Number of command line arguments.
 * \param[in] argv Command line arguments.
 * \param[out] sample_image_index Index of the sample image to be generated.
 * \retval true Initialization succeeded.
 * \retval false Initialization failed.
 */
[[nodiscard]] inline auto initialize(
    int argc, char** argv, int& sample_image_index) -> bool {
    auto log_tag_config =
        num_collect::logging::log_tag_config()
            .output_log_level(num_collect::logging::log_level::debug)
            .output_log_level_in_child_iterations(
                num_collect::logging::log_level::warning);
    num_collect::logging::set_default_tag_config(log_tag_config);
    log_tag_config.iteration_output_period(1);
    num_collect::logging::set_config_of(
        num_collect::opt::gaussian_process_optimizer_tag, log_tag_config);

    sample_image_index = 1;
    const auto cli =
        lyra::cli().add_argument(lyra::opt(sample_image_index, "index")
                .name("--image-index")
                .name("-i")
                .optional()
                .help("Index of the sample image to be generated. "
                      "1 (default): One constant circle."));
    const auto result = cli.parse({argc, argv});
    if (!result) {
        std::cerr << result.message() << "\n\n";
        std::cerr << cli << std::endl;
        return false;
    }

    return true;
}

/*!
 * \brief Generate a sample image matrix.
 *
 * \param[in] sample_image_index Index of the sample image to be generated.
 * \param[out] origin Original image matrix.
 * \retval true Sample image generated successfully.
 * \retval false Failed to generate the sample image.
 */
[[nodiscard]] inline auto generate_sample_image(
    int sample_image_index, Eigen::MatrixXd& origin) -> bool {
    switch (sample_image_index) {
    case 1:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_constant_circle(
                origin, rows, cols);
        break;
        // TODO more sample images
    default:
        std::cerr << "Unknown sample image index: " << sample_image_index
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
