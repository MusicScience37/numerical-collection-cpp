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
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>
#include <png++/types.hpp>

#include "num_collect/base/index_type.h"
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
    constexpr num_collect::index_type size = rows * cols;

    const Eigen::Vector2d center = Eigen::Vector2d(0.7, 0.6);
    constexpr double radius = 0.2;
    Eigen::MatrixXd origin = Eigen::MatrixXd::Zero(rows, cols);
    add_circle(origin, center, radius);
    write_image(origin, "./sparse_image_origin.png");

    constexpr double noise_rate = 0.1;
    Eigen::MatrixXd data = origin;
    add_noise(data, noise_rate);
    write_image(data, "./sparse_image_data.png");

    using coeff_type = Eigen::SparseMatrix<double>;
    coeff_type coeff;
    coeff.resize(size, size);
    coeff.setIdentity();

    constexpr double reg_param = 0.2;
    num_collect::regularization::fista<coeff_type, Eigen::VectorXd> solver;
    const Eigen::VectorXd data_vec = data.reshaped();
    solver.compute(coeff, data_vec);
    Eigen::VectorXd solution_vec = data_vec;
    solver.solve(reg_param, solution_vec);

    const Eigen::MatrixXd solution = solution_vec.reshaped(rows, cols);
    write_image(solution, "./sparse_image_solution.png");

    const Eigen::MatrixXd error = (solution - origin).cwiseAbs();
    write_image(error, "./sparse_image_error.png");

    return 0;
}
