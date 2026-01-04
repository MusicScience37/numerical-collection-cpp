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
 * \brief Definition of functions to write PNG files for image denoising.
 */
#pragma once

#include <string>

#include <Eigen/Core>
#include <png++/gray_pixel.hpp>
#include <png++/image.hpp>
#include <png++/types.hpp>

#include "num_collect/logging/logger.h"
#include "num_collect/util/assert.h"
#include "write_png.h"

/*!
 * \brief Write PNG files of the result.
 *
 * \param[in] origin Original image.
 * \param[in] data Data image with noise.
 * \param[in] solution Solution image.
 * \param[in] algorithm_name_for_file_name Name of the algorithm used for
 * solving, for file name of the plot.
 */
inline void write_png(const Eigen::MatrixXd& origin,
    const Eigen::MatrixXd& data, const Eigen::MatrixXd& solution,
    const std::string& algorithm_name_for_file_name) {
    NUM_COLLECT_ASSERT(origin.rows() == data.rows());
    NUM_COLLECT_ASSERT(origin.cols() == data.cols());
    NUM_COLLECT_ASSERT(origin.rows() == solution.rows());
    NUM_COLLECT_ASSERT(origin.cols() == solution.cols());

    num_collect::logging::logger logger;
    write_png(origin,
        fmt::format(
            "./image_denoising_{}_origin.png", algorithm_name_for_file_name));
    write_png(data,
        fmt::format(
            "./image_denoising_{}_data.png", algorithm_name_for_file_name));
    write_png(solution,
        fmt::format(
            "./image_denoising_{}_solution.png", algorithm_name_for_file_name));
}
