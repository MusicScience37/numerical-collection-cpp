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
 * \brief Definition of functions to write PNG files for CT simulation.
 */
#pragma once

#include <string>

#include <Eigen/Core>
#include <png++/gray_pixel.hpp>
#include <png++/image.hpp>
#include <png++/types.hpp>

#include "num_collect/logging/logger.h"
#include "num_collect/util/assert.h"

/*!
 * \brief Write a PNG file of a single matrix.
 *
 * \param[in] matrix Matrix to write.
 * \param[in] filepath File path to write.
 * \param[in] logger Logger.
 */
inline void write_png_one(const Eigen::MatrixXd& matrix,
    const std::string& filepath, const num_collect::logging::logger& logger) {
    using png_index_type = png::uint_32;

    png::image<png::gray_pixel> image(
        static_cast<png_index_type>(matrix.cols()),
        static_cast<png_index_type>(matrix.rows()));
    constexpr double max_pixel_value = 255.0;
    constexpr double min_pixel_value = 0.0;
    constexpr double scale = max_pixel_value;
    for (num_collect::index_type i = 0; i < matrix.rows(); ++i) {
        for (num_collect::index_type j = 0; j < matrix.cols(); ++j) {
            const double raw_pixel_value = scale * matrix(i, j);
            const auto clipped_pixel_value = static_cast<std::uint8_t>(std::max(
                min_pixel_value, std::min(max_pixel_value, raw_pixel_value)));
            image[static_cast<png_index_type>(j)]
                 [static_cast<png_index_type>(i)] = clipped_pixel_value;
        }
    }

    image.write(filepath);
    logger.info()("Wrote {}.", filepath);
}

/*!
 * \brief Write PNG files of the result.
 *
 * \param[in] origin Original image.
 * \param[in] solution Solution image.
 * \param[in] algorithm_name_for_file_name Name of the algorithm used for
 * solving, for file name of the plot.
 */
inline void write_png(const Eigen::MatrixXd& origin,
    const Eigen::MatrixXd& solution,
    const std::string& algorithm_name_for_file_name) {
    NUM_COLLECT_ASSERT(origin.rows() == solution.rows());
    NUM_COLLECT_ASSERT(origin.cols() == solution.cols());

    num_collect::logging::logger logger;
    write_png_one(origin,
        fmt::format(
            "./ct_simulation_{}_origin.png", algorithm_name_for_file_name),
        logger);
    write_png_one(solution,
        fmt::format(
            "./ct_simulation_{}_solution.png", algorithm_name_for_file_name),
        logger);
}
