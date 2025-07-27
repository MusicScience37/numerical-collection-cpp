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
 * \brief Definition of functions to generate sparse sample images.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Add a circle with constant value to an image.
 *
 * \param[out] image Image to add a circle.
 * \param[in] center Center of the circle to be drawn.
 * \param[in] radius Radius of the circle to be drawn.
 * \param[in] value Value to be set in the circle.
 */
inline void add_constant_circle(Eigen::MatrixXd& image,
    const Eigen::Vector2d& center, double radius, double value = 1.0) {
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
                image(i, j) = value;
            }
        }
    }
}

/*!
 * \brief Add a circle with values given by a quadratic function to an image.
 *
 * \param[out] image Image to add a circle.
 * \param[in] center Center of the circle to be drawn.
 * \param[in] radius Radius of the circle to be drawn.
 * \param[in] center_value Value at the center of the circle.
 */
inline void add_quadratic_circle(Eigen::MatrixXd& image,
    const Eigen::Vector2d& center, double radius, double center_value = 1.0) {
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
            const double dist_ratio = dist / radius;
            image(i, j) = std::max(
                image(i, j), center_value * (1.0 - dist_ratio * dist_ratio));
        }
    }
}

/*!
 * \brief Add a circle with values given by a smooth function to an image.
 *
 * \param[out] image Image to add a circle.
 * \param[in] center Center of the circle to be drawn.
 * \param[in] radius Radius of the circle to be drawn.
 * \param[in] center_value Value at the center of the circle.
 *
 * This function uses Wendland's compactly supported radial basis function for a
 * smooth function.
 */
inline void add_smooth_circle(Eigen::MatrixXd& image,
    const Eigen::Vector2d& center, double radius, double center_value = 1.0) {
    const num_collect::index_type rows = image.rows();
    const num_collect::index_type cols = image.cols();

    const num_collect::rbf::rbfs::wendland_csrbf<double, 3, 1> rbf;
    const double scaling_factor = center_value / rbf(0.0);

    Eigen::Vector2d point;
    for (num_collect::index_type j = 0; j < cols; ++j) {
        const double x = static_cast<double>(j) / static_cast<double>(cols - 1);
        point.x() = x;
        for (num_collect::index_type i = 0; i < rows; ++i) {
            const double y =
                static_cast<double>(i) / static_cast<double>(rows - 1);
            point.y() = y;
            const double dist = (point - center).norm();
            const double dist_ratio = dist / radius;
            image(i, j) =
                std::max(image(i, j), scaling_factor * rbf(dist_ratio));
        }
    }
}

/*!
 * \brief Generate a sparse sample image with one constant circle.
 *
 * \param[out] image Image to be generated.
 * \param[in] rows Number of rows of the image.
 * \param[in] cols Number of columns of the image.
 */
inline void generate_sparse_sample_image_with_one_constant_circle(
    Eigen::MatrixXd& image, num_collect::index_type rows,
    num_collect::index_type cols) {
    const Eigen::Vector2d center = Eigen::Vector2d(0.7, 0.6);
    constexpr double radius = 0.2;
    image = Eigen::MatrixXd::Zero(rows, cols);
    add_constant_circle(image, center, radius);
}

/*!
 * \brief Generate a sparse sample image with two constant circles.
 *
 * \param[out] image Image to be generated.
 * \param[in] rows Number of rows of the image.
 * \param[in] cols Number of columns of the image.
 */
inline void generate_sparse_sample_image_with_two_constant_circles(
    Eigen::MatrixXd& image, num_collect::index_type rows,
    num_collect::index_type cols) {
    const Eigen::Vector2d center1 = Eigen::Vector2d(0.3, 0.4);
    constexpr double radius1 = 0.1;
    constexpr double value1 = 0.5;

    const Eigen::Vector2d center2 = Eigen::Vector2d(0.7, 0.6);
    constexpr double radius2 = 0.2;
    constexpr double value2 = 1.0;

    image = Eigen::MatrixXd::Zero(rows, cols);
    add_constant_circle(image, center1, radius1, value1);
    add_constant_circle(image, center2, radius2, value2);
}

/*!
 * \brief Generate a sparse sample image with one quadratic circle.
 *
 * \param[out] image Image to be generated.
 * \param[in] rows Number of rows of the image.
 * \param[in] cols Number of columns of the image.
 */
inline void generate_sparse_sample_image_with_one_quadratic_circle(
    Eigen::MatrixXd& image, num_collect::index_type rows,
    num_collect::index_type cols) {
    const Eigen::Vector2d center = Eigen::Vector2d(0.7, 0.6);
    constexpr double radius = 0.2;
    constexpr double center_value = 1.0;

    image = Eigen::MatrixXd::Zero(rows, cols);
    add_quadratic_circle(image, center, radius, center_value);
}

/*!
 * \brief Generate a sparse sample image with one smooth circle.
 *
 * \param[out] image Image to be generated.
 * \param[in] rows Number of rows of the image.
 * \param[in] cols Number of columns of the image.
 */
inline void generate_sparse_sample_image_with_one_smooth_circle(
    Eigen::MatrixXd& image, num_collect::index_type rows,
    num_collect::index_type cols) {
    const Eigen::Vector2d center = Eigen::Vector2d(0.7, 0.6);
    constexpr double radius = 0.3;
    constexpr double center_value = 1.0;

    image = Eigen::MatrixXd::Zero(rows, cols);
    add_smooth_circle(image, center, radius, center_value);
}

}  // namespace num_prob_collect::regularization
