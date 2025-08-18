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
 * \brief Definition of common functions for CT simulation.
 */
#pragma once

#include <iostream>
#include <optional>
#include <string>

#include <Eigen/Core>
#include <fmt/format.h>
#include <lyra/lyra.hpp>
#include <toml++/toml.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_prob_collect/regularization/generate_sparse_sample_image.h"

struct ct_simulation_config {
    num_collect::index_type image_size;
    num_collect::index_type num_directions;
    num_collect::index_type num_rays_per_direction;
    double noise_rate;
    int sample_image_index;
};

/*!
 * \brief Perform common initialization for CT simulation examples.
 *
 * \param[in] argc Number of command line arguments.
 * \param[in] argv Command line arguments.
 * \return Configuration for CT simulation or `std::nullopt` if failed.
 */
[[nodiscard]] inline auto initialize(int argc, char** argv)
    -> std::optional<ct_simulation_config> {
#ifndef NDEBUG
    std::string config_file_path =
        "../../examples/regularization/ct_simulation_config_small.toml";
#else
    std::string config_file_path =
        "../../examples/regularization/ct_simulation_config.toml";
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
    ct_simulation_config config;
    try {
        config.image_size = config_table.at_path("ct_simulation.image_size")
                                .value<num_collect::index_type>()
                                .value();
        config.num_directions =
            config_table.at_path("ct_simulation.num_directions")
                .value<num_collect::index_type>()
                .value();
        config.num_rays_per_direction =
            config_table.at_path("ct_simulation.num_rays_per_direction")
                .value<num_collect::index_type>()
                .value();
        config.noise_rate = config_table.at_path("ct_simulation.noise_rate")
                                .value<double>()
                                .value();
        config.sample_image_index =
            config_table.at_path("ct_simulation.sample_image_index")
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
    const ct_simulation_config& config, Eigen::MatrixXd& origin) -> bool {
    switch (config.sample_image_index) {
    case 1:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_constant_circle(
                origin, config.image_size, config.image_size);
        break;
    case 2:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_two_constant_circles(
                origin, config.image_size, config.image_size);
        break;
    case 3:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_quadratic_circle(
                origin, config.image_size, config.image_size);
        break;
    case 4:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_one_smooth_circle(
                origin, config.image_size, config.image_size);
        break;
    case 5:
        num_prob_collect::regularization::
            generate_sparse_sample_image_with_two_smooth_circles(
                origin, config.image_size, config.image_size);
        break;
    default:
        std::cerr << "Unknown sample image index: " << config.sample_image_index
                  << std::endl;
        return false;
    }
    return true;
}
