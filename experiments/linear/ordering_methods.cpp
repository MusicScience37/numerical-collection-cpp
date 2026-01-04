/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Example of ordering methods for sparse matrices.
 */
#include <algorithm>
#include <random>
#include <string>

#include <Eigen/Core>
#include <Eigen/OrderingMethods>
#include <Eigen/SparseCore>
#include <fmt/format.h>

#include "num_collect/base/index_type.h"
#include "num_collect/linear/cuthill_mckee_ordering.h"
#include "num_collect/linear/reverse_cuthill_mckee_ordering.h"
#include "num_collect/logging/logger.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"
#include "write_png.h"

using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
using vec_type = Eigen::VectorXd;
using grid_type = num_prob_collect::linear::laplacian_2d_grid<mat_type>;

static void write_image(const mat_type& matrix, const std::string& filepath) {
    // Change to matrix of 0 or 1 for visualization.
    Eigen::MatrixXd output_matrix =
        Eigen::MatrixXd::Zero(matrix.rows(), matrix.cols());
    for (num_collect::index_type i = 0; i < matrix.rows(); ++i) {
        for (typename mat_type::InnerIterator it(matrix, i); it; ++it) {
            output_matrix(i, it.col()) = 1.0;
        }
    }
    write_png(output_matrix, filepath);
}

static void randomize(const mat_type& input, mat_type& output) {
    const num_collect::index_type size = input.rows();
    Eigen::VectorXi indices;
    indices.resize(size);
    for (num_collect::index_type i = 0; i < size; ++i) {
        indices(i) = static_cast<int>(i);
    }

    std::random_device seed_generator;
    std::mt19937 generator(seed_generator());
    std::shuffle(indices.begin(), indices.end(), generator);

    Eigen::PermutationMatrix<Eigen::Dynamic> permutation(indices);
    output = input.twistedBy(permutation);
}

template <typename Ordering>
static void test_ordering(
    const mat_type& matrix, const std::string& method_name) {
    Ordering ordering;
    Eigen::PermutationMatrix<Eigen::Dynamic> permutation;
    ordering(matrix, permutation);
    mat_type ordered_matrix;
    ordered_matrix = matrix.twistedBy(permutation);
    write_image(ordered_matrix, fmt::format("./ordering_{}.png", method_name));
}

auto main() -> int {
    num_collect::logging::logger logger;

    constexpr num_collect::index_type grid_size = 10;
    constexpr double grid_width = 1.0 / static_cast<double>(grid_size);
    logger.info()("grid_size: {}", grid_size);

    grid_type grid{grid_size - 1, grid_size - 1, grid_width};
    logger.info()("Generated grid.");

    mat_type matrix;
    randomize(grid.mat(), matrix);
    write_image(matrix, "./ordering_original.png");

    test_ordering<Eigen::COLAMDOrdering<int>>(matrix, "colamd");
    test_ordering<num_collect::linear::cuthill_mckee_ordering<int>>(
        matrix, "cuthill_mckee");
    test_ordering<num_collect::linear::reverse_cuthill_mckee_ordering<int>>(
        matrix, "reverse_cuthill_mckee");

    return 0;
}
