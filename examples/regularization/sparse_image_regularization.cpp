/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Example of tikhonov class with blurred sine test problem.
 */
#include <Eigen/Core>
#include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>

#include "num_collect/base/index_type.h"

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

static void write_image(
    const Eigen::MatrixXd& image_mat, const std::string& filepath) {
    const num_collect::index_type rows = image_mat.rows();
    const num_collect::index_type cols = image_mat.cols();

    using png_index_type = png::uint_32;

    png::image<png::rgb_pixel> image(
        static_cast<png_index_type>(rows), static_cast<png_index_type>(cols));
    for (num_collect::index_type j = 0; j < cols; ++j) {
        for (num_collect::index_type i = 0; i < rows; ++i) {
            const double raw_val = image_mat(i, j);
            constexpr double max_val = 255.0;
            const auto val = static_cast<std::uint8_t>(static_cast<int>(
                std::min(max_val, std::max(0.0, max_val * raw_val))));

            image[static_cast<png_index_type>(i)][static_cast<png_index_type>(
                j)] = png::rgb_pixel(val, val, val);
        }
    }

    image.write(filepath);
}

auto main() -> int {
    constexpr num_collect::index_type rows = 20;
    constexpr num_collect::index_type cols = 20;
    Eigen::MatrixXd origin = Eigen::MatrixXd::Zero(rows, cols);

    const Eigen::Vector2d center = Eigen::Vector2d(0.7, 0.6);
    constexpr double radius = 0.2;
    add_circle(origin, center, radius);

    write_image(origin, "./sparse_image_origin.png");

    return 0;
}
