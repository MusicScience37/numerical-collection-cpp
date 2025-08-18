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
 * \brief Profiling of tgv2_admm class using CT simulation.
 */
#include <string>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <gperftools/profiler.h>

#include "ct_simulation_common.h"
#include "num_collect/regularization/implicit_gcv.h"
#include "num_collect/regularization/tgv2_admm.h"
#include "num_prob_collect/regularization/add_noise.h"
#include "num_prob_collect/regularization/sparse_ct_matrix_2d.h"
#include "num_prob_collect/regularization/sparse_diff_matrix_2d.h"
#include "num_prob_collect/regularization/tgv2_second_derivative_matrix_2d.h"

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
        num_prob_collect::regularization::sparse_ct_matrix_2d<coeff_type>(
            config.image_size, config.num_directions,
            config.num_rays_per_direction);

    // Prepare data with noise.
    Eigen::VectorXd data = coeff * origin_vec;
    num_prob_collect::regularization::add_noise(data, config.noise_rate);

    // Prepare a matrix for the 1st order derivative operator.
    using derivative_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    const auto first_derivative_matrix =
        num_prob_collect::regularization::sparse_diff_matrix_2d<
            derivative_matrix_type>(config.image_size, config.image_size);

    // Prepare a matrix for the 2nd order derivative operator.
    const auto second_derivative_matrix =
        num_prob_collect::regularization::tgv2_second_derivative_matrix_2d<
            derivative_matrix_type>(config.image_size, config.image_size);

    // Prepare a solver.
    using solver_type = num_collect::regularization::tgv2_admm<coeff_type,
        derivative_matrix_type, Eigen::VectorXd>;
    solver_type solver;
    solver.compute(
        coeff, first_derivative_matrix, second_derivative_matrix, data);

    // Search for an optimal regularization parameter.
    const Eigen::VectorXd initial_solution =
        Eigen::VectorXd::Zero(config.image_size * config.image_size);
    num_collect::regularization::implicit_gcv<solver_type> gcv{
        solver, data, initial_solution};

    ProfilerStart("profile_reg_ct_simulation_tgv2_admm.prof");
    gcv.search();
    ProfilerStop();

    // Solve the problem using the optimal parameter.
    Eigen::VectorXd solution_vec = initial_solution;
    gcv.solve(solution_vec);

    // Reshape the solution vector to a matrix for visualization.
    // The solution is the denoised image.
    const Eigen::MatrixXd solution = solution_vec.reshaped<Eigen::ColMajor>(
        config.image_size, config.image_size);

    return 0;
}
