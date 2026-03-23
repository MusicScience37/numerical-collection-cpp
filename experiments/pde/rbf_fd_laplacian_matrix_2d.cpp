/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Experiments to compute matrices of Laplacian using RBF-FD in 2D.
 */
#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/impl/rbf_fd_row_calculator.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector_view.h"

using variable_type = Eigen::Vector2d;

constexpr num_collect::index_type num_nodes = 50;
constexpr num_collect::index_type num_neighbors = 20;

static auto test_function(const variable_type& variable) -> double {
    return variable.array().sin().prod();
}

static auto test_function_laplacian(const variable_type& variable) -> double {
    return -2.0 * variable.array().sin().prod();
}

static void evaluate_laplacian_matrix(
    num_collect::util::vector_view<const variable_type> nodes,
    const Eigen::SparseMatrix<double>& laplacian_matrix) {
    Eigen::VectorXd function_values(nodes.size());
    Eigen::VectorXd true_laplacian_values(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        function_values(i) = test_function(nodes[i]);
        true_laplacian_values(i) = test_function_laplacian(nodes[i]);
    }
    Eigen::VectorXd approximated_laplacian_values =
        laplacian_matrix * function_values;

    const Eigen::VectorXd errors =
        approximated_laplacian_values - true_laplacian_values;
    const double max_error = errors.cwiseAbs().maxCoeff();
    const double mean_error = errors.cwiseAbs().mean();
    const double max_value = true_laplacian_values.cwiseAbs().maxCoeff();

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger,
        "Max error: {:.2e}, Mean error: {:.2e}, Max value: {:.2e}", max_error,
        mean_error, max_value);
}

static void test_laplacian_matrix_without_polynomial(
    num_collect::util::vector_view<const variable_type> nodes) {
    using distance_function_type =
        num_collect::rbf::distance_functions::euclidean_distance_function<
            variable_type>;
    using rbf_type = num_collect::rbf::rbfs::gaussian_rbf<double>;
    using length_parameter_calculator_type =
        num_collect::rbf::length_parameter_calculators::
            global_length_parameter_calculator<distance_function_type>;
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger,
        "Start computation of the Laplacian matrix without polynomials.");

    const distance_function_type distance_function;
    const rbf_type rbf;
    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);

    // TODO Replace with a class to calculate the whole matrix later.
    num_collect::rbf::impl::rbf_fd_row_calculator<
        length_parameter_calculator_type>
        row_calculator;

    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        const auto target_operator = operator_type{nodes[i]};
        row_calculator.compute_row(distance_function, rbf, target_operator,
            nodes[i], nodes, column_variables_nearest_neighbor_searcher,
            num_neighbors, triplets, i);
    }
    Eigen::SparseMatrix<double> laplacian_matrix(nodes.size(), nodes.size());
    laplacian_matrix.setFromTriplets(triplets.begin(), triplets.end());

    NUM_COLLECT_LOG_INFO(logger,
        "Finished computation of the Laplacian matrix without polynomials.");

    evaluate_laplacian_matrix(nodes, laplacian_matrix);
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_laplacian_matrix_2d.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);

    const auto nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_nodes);

    test_laplacian_matrix_without_polynomial(nodes);

    return 0;
}
