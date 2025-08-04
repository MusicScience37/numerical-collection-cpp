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
 * \brief Definition of sparse_ct_matrix_2d function.
 */
#pragma once

#include <cmath>
#include <type_traits>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/constants/pi.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Create a sparse matrix to simulate calculation of data in CT (computed
 * tomography) in 2D.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] image_size Size of the image. (Rows and columns are the same to
 * this.)
 * \param[in] num_directions Number of directions of centers of rays.
 * \param[in] num_rays_per_direction Number of rays per direction.
 * \return Created sparse matrix.
 *
 * This function creates a coefficient matrix to calculate data in CT from an
 * image in 2D.
 * Images are stored in a vector in column-major order.
 *
 * This function simulates the following setup:
 * - The image is in the range of [-0.5, 0.5] in both x and y axes.
 * - Rays are injected from points on a circle of radius 1.0 centered at the
 * origin.
 */
template <num_collect::concepts::sparse_matrix Matrix>
[[nodiscard]] auto sparse_ct_matrix_2d(num_collect::index_type image_size,
    num_collect::index_type num_directions,
    num_collect::index_type num_rays_per_direction) -> Matrix {
    using scalar_type = typename Matrix::value_type;
    using storage_index_type = typename Matrix::StorageIndex;

    static_assert(std::is_same_v<scalar_type, double>,
        "Currently, only double is supported for scalars.");
    NUM_COLLECT_PRECONDITION(image_size > 0, "Image size must be positive.");
    NUM_COLLECT_PRECONDITION(
        num_directions > 0, "Number of directions must be positive.");
    NUM_COLLECT_PRECONDITION(num_rays_per_direction >= 2,
        "Number of rays per direction must be at least 2.");

    const num_collect::index_type rows =
        num_rays_per_direction * num_directions;
    const num_collect::index_type cols = image_size * image_size;

    constexpr double ray_center_radius = 1.0;
    constexpr double rays_angle = num_collect::constants::pi<double> / 6.0;
    const double delta_function_width = 1.0 / static_cast<double>(image_size);

    std::vector<Eigen::Triplet<scalar_type, storage_index_type>> triplets;
    for (num_collect::index_type direction_index = 0;
        direction_index < num_directions; ++direction_index) {
        const double center_angle = num_collect::constants::pi<double> *
            static_cast<double>(direction_index) /
            static_cast<double>(num_directions);
        const Eigen::Vector2d center =
            Eigen::Vector2d(ray_center_radius * std::cos(center_angle),
                ray_center_radius * std::sin(center_angle));
        for (num_collect::index_type ray_index = 0;
            ray_index < num_rays_per_direction; ++ray_index) {
            const double ray_angle = center_angle +
                num_collect::constants::pi<double> +
                rays_angle *
                    ((static_cast<double>(ray_index) /
                         static_cast<double>(num_rays_per_direction - 1)) *
                            2.0 -
                        1.0);
            const Eigen::Vector2d direction =
                Eigen::Vector2d(std::cos(ray_angle), std::sin(ray_angle));
            const Eigen::Vector2d direction_normal =
                Eigen::Vector2d(-direction.y(), direction.x());
            const num_collect::index_type row =
                direction_index * num_rays_per_direction + ray_index;
            for (num_collect::index_type image_col = 0; image_col < image_size;
                ++image_col) {
                const double x = (static_cast<double>(image_col) + 0.5) /
                        static_cast<double>(image_size) -
                    0.5;
                for (num_collect::index_type image_row = 0;
                    image_row < image_size; ++image_row) {
                    const double y = (static_cast<double>(image_row) + 0.5) /
                            static_cast<double>(image_size) -
                        0.5;
                    const Eigen::Vector2d point(x, y);
                    const double distance_from_ray =
                        std::abs((point - center).dot(direction_normal));
                    if (distance_from_ray < delta_function_width) {
                        // Image is column major.
                        const num_collect::index_type col =
                            image_col * image_size + image_row;
                        const double value =
                            (1.0 - distance_from_ray / delta_function_width) /
                            delta_function_width;
                        triplets.emplace_back(row, col, value);
                    }
                }
            }
        }
    }

    Matrix matrix(rows, cols);
    matrix.setFromTriplets(triplets.begin(), triplets.end());
    return matrix;
}

}  // namespace num_prob_collect::regularization
