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
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_assembler.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector_view.h"

using variable_type = Eigen::Vector2d;

constexpr num_collect::index_type num_nodes = 100;
constexpr num_collect::index_type num_neighbors = 15;

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
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;
    using assembler_type = num_collect::rbf::rbf_fd_assembler<variable_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger,
        "Start computation of the Laplacian matrix without polynomials.");

    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);

    assembler_type assembler;
    assembler.num_neighbors(num_neighbors);

    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    assembler.compute_rows<operator_type>(nodes, nodes,
        column_variables_nearest_neighbor_searcher, triplets, 0, 0);
    Eigen::SparseMatrix<double> laplacian_matrix(nodes.size(), nodes.size());
    laplacian_matrix.setFromTriplets(triplets.begin(), triplets.end());

    NUM_COLLECT_LOG_INFO(logger,
        "Finished computation of the Laplacian matrix without polynomials.");

    evaluate_laplacian_matrix(nodes, laplacian_matrix);
}

static void test_laplacian_matrix_with_polynomial(
    num_collect::util::vector_view<const variable_type> nodes,
    int polynomial_degree) {
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;
    using assembler_type =
        num_collect::rbf::rbf_fd_polynomial_assembler<variable_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger,
        "Start computation of the Laplacian matrix with polynomials of degree "
        "{}.",
        polynomial_degree);

    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);

    assembler_type assembler(polynomial_degree);
    assembler.num_neighbors(num_neighbors);

    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    assembler.compute_rows<operator_type>(nodes, nodes,
        column_variables_nearest_neighbor_searcher, triplets, 0, 0);
    Eigen::SparseMatrix<double> laplacian_matrix(nodes.size(), nodes.size());
    laplacian_matrix.setFromTriplets(triplets.begin(), triplets.end());

    NUM_COLLECT_LOG_INFO(logger,
        "Finished computation of the Laplacian matrix with polynomials of "
        "degree "
        "{}.",
        polynomial_degree);

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
    test_laplacian_matrix_with_polynomial(nodes, 0);
    test_laplacian_matrix_with_polynomial(nodes, 1);
    test_laplacian_matrix_with_polynomial(nodes, 2);
    test_laplacian_matrix_with_polynomial(nodes, 3);

    return 0;
}
