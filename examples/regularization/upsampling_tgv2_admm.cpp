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
 * \brief Example of upsampling using tv_admm class.
 */
#include <string>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "num_collect/regularization/implicit_gcv.h"
#include "num_collect/regularization/tgv2_admm.h"
#include "num_prob_collect/regularization/sparse_diff_matrix_2d.h"
#include "num_prob_collect/regularization/sparse_downsampling_matrix_2d.h"
#include "num_prob_collect/regularization/tgv2_second_derivative_matrix_2d.h"
#include "upsampling_common.h"

auto main(int argc, char** argv) -> int {
    // Perform common initialization for examples.
    const auto config_result = initialize(argc, argv);
    if (!config_result) {
        return 1;
    }

    const auto& config = *config_result;

    // Generate the original image.
    Eigen::MatrixXd origin;
    if (!generate_sample_image(config, origin)) {
        return 1;
    }

    // Reshape the original image to a vector for processing.
    const Eigen::VectorXd origin_vec = origin.reshaped<Eigen::ColMajor>();

    // Prepare a coefficient matrix from parameters to data.
    using coeff_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    const coeff_type coeff =
        num_prob_collect::regularization::sparse_downsampling_matrix_2d<
            coeff_type>(config.upsampled_cols, config.upsampled_rows,
            config.original_cols, config.original_rows);

    // Prepare a matrix for the 1st order derivative operator.
    using derivative_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    const auto first_derivative_matrix =
        num_prob_collect::regularization::sparse_diff_matrix_2d<
            derivative_matrix_type>(
            config.upsampled_cols, config.upsampled_rows);

    // Prepare a matrix for the 2nd order derivative operator.
    const auto second_derivative_matrix =
        num_prob_collect::regularization::tgv2_second_derivative_matrix_2d<
            derivative_matrix_type>(
            config.upsampled_cols, config.upsampled_rows);

    // Prepare a solver.
    using solver_type = num_collect::regularization::tgv2_admm<coeff_type,
        derivative_matrix_type, Eigen::VectorXd>;
    solver_type solver;
    solver.compute(
        coeff, first_derivative_matrix, second_derivative_matrix, origin_vec);

    // Prepare an initial solution vector.
    const Eigen::VectorXd initial_solution =
        coeff.transpose() * origin_vec / coeff.coeff(0, 0);

    // Search for an optimal regularization parameter.
    num_collect::regularization::implicit_gcv<solver_type> gcv{
        solver, origin_vec, initial_solution};
    gcv.search();

    // Solve the problem using the optimal parameter.
    Eigen::VectorXd solution_vec = initial_solution;
    gcv.solve(solution_vec);

    // Reshape the solution vector to a matrix for visualization.
    Eigen::MatrixXd upsampled = solution_vec.reshaped<Eigen::ColMajor>(
        config.upsampled_rows, config.upsampled_cols);

    // Visualize the result.
    visualize_result(origin, upsampled, "TGV2 Regularization", "tgv2_admm");

    return 0;
}
