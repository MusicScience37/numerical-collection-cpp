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
 * \brief Example of image denoising using tgv2_admm class.
 */
#include <string>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "image_denoising_common.h"
#include "num_collect/base/index_type.h"
#include "num_collect/regularization/implicit_gcv.h"
#include "num_collect/regularization/tgv2_admm.h"
#include "num_prob_collect/regularization/add_noise.h"
#include "num_prob_collect/regularization/sparse_diff_matrix_2d.h"
#include "num_prob_collect/regularization/tgv2_second_derivative_matrix_2d.h"

auto main(int argc, char** argv) -> int {
    // Perform common initialization for examples.
    const auto config_result = initialize(argc, argv);
    if (!config_result) {
        return 1;
    }

    const auto& config = *config_result;
    const num_collect::index_type size = config.rows * config.cols;

    // Generate the original image.
    Eigen::MatrixXd origin;
    if (!generate_sample_image(config, origin)) {
        return 1;
    }

    // Prepare data with noise.
    Eigen::MatrixXd data = origin;
    num_prob_collect::regularization::add_noise(data, config.noise_rate);

    // Reshape the data to a vector for processing.
    const Eigen::VectorXd data_vec = data.reshaped<Eigen::ColMajor>();

    // Prepare a coefficient matrix from parameters to data.
    // For denoising, the coefficient matrix is an identity matrix.
    using coeff_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    coeff_type coeff;
    coeff.resize(size, size);
    coeff.setIdentity();

    // Prepare a matrix for the 1st order derivative operator.
    using derivative_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    const auto first_derivative_matrix =
        num_prob_collect::regularization::sparse_diff_matrix_2d<
            derivative_matrix_type>(config.cols, config.rows);

    // Prepare a matrix for the 2nd order derivative operator.
    const auto second_derivative_matrix =
        num_prob_collect::regularization::tgv2_second_derivative_matrix_2d<
            derivative_matrix_type>(config.cols, config.rows);

    // Prepare a solver.
    using solver_type = num_collect::regularization::tgv2_admm<coeff_type,
        derivative_matrix_type, Eigen::VectorXd>;
    solver_type solver;
    solver.compute(
        coeff, first_derivative_matrix, second_derivative_matrix, data_vec);

    // Search for an optimal regularization parameter.
    const auto& initial_solution = data_vec;
    num_collect::regularization::implicit_gcv<solver_type> gcv{
        solver, data_vec, initial_solution};
    gcv.search();

    // Solve the problem using the optimal parameter.
    Eigen::VectorXd solution_vec = initial_solution;
    gcv.solve(solution_vec);

    // Reshape the solution vector to a matrix for visualization.
    // The solution is the denoised image.
    const Eigen::MatrixXd solution =
        solution_vec.reshaped<Eigen::ColMajor>(config.rows, config.cols);

    // Visualize the result.
    visualize_result(
        origin, data, solution, "TGV2 Regularization", "tgv2_admm");

    return 0;
}
