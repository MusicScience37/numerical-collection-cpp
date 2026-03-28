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
 * \brief Test of different dimensions of the Poisson equation with
 * phs_rbf_fd_polynomial_assembler class.
 */
#include <cmath>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "eigen_approx.h"
#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

TEMPLATE_TEST_CASE_SIG(
    "Poisson equation with phs_rbf_fd_polynomial_assembler in 2D with "
    "polynomials",
    "", ((int Degree), Degree), (0), (1), (2), (3)) {
    using variable_type = Eigen::Vector2d;
    using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<variable_type>;

    auto test_function = [](const variable_type& variable) -> double {
        return (variable.array() * num_collect::base::pi<double>).sin().prod();
    };
    auto test_function_laplacian = [](const variable_type& variable) -> double {
        constexpr double coeff = -2.0 * num_collect::base::pi<double> *
            num_collect::base::pi<double>;
        return coeff *
            (variable.array() * num_collect::base::pi<double>).sin().prod();
    };

    num_collect::logging::logger logger;

    NUM_COLLECT_LOG_DEBUG(logger, "Generate nodes.");
    constexpr num_collect::index_type num_interior_nodes = 100;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 10;
    auto nodes =
        num_collect::rbf::generate_halton_nodes<typename variable_type::Scalar,
            variable_type::RowsAtCompileTime>(num_interior_nodes);
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            variable_type(0.0, 0.0), variable_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    const auto interior_nodes =
        num_collect::util::vector_view<const variable_type>(nodes).first(
            num_interior_nodes);

    NUM_COLLECT_LOG_DEBUG(logger, "Assemble the system.");
    assembler_type assembler(Degree);
    assembler.num_neighbors(15);
    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd right_vec(nodes.size());
    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);
    assembler.compute_rows<operator_type>(interior_nodes, nodes,
        column_variables_nearest_neighbor_searcher, triplets, 0, 0);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        right_vec(i) = test_function_laplacian(interior_nodes[i]);
    }
    for (num_collect::index_type i = num_interior_nodes; i < nodes.size();
        ++i) {
        const auto index_in_triplet = static_cast<int>(i);
        triplets.emplace_back(index_in_triplet, index_in_triplet, 1.0);
        right_vec(i) = test_function(nodes[i]);
    }
    sparse_matrix_type mat(nodes.size(), nodes.size());
    mat.setFromTriplets(triplets.begin(), triplets.end());

    NUM_COLLECT_LOG_DEBUG(logger, "Solve the system.");
    Eigen::BiCGSTAB<sparse_matrix_type> solver;
    solver.compute(mat);
    const Eigen::VectorXd solution = solver.solve(right_vec);

    NUM_COLLECT_LOG_DEBUG(logger, "Evaluate the solution.");
    Eigen::VectorXd true_values(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        true_values(i) = test_function(nodes[i]);
    }
    if constexpr (Degree == 0) {
        CHECK_THAT(solution, eigen_approx(true_values, 0.1));
    } else if constexpr (Degree == 1) {
        CHECK_THAT(solution, eigen_approx(true_values, 0.1));
    } else if constexpr (Degree == 2) {
        CHECK_THAT(solution, eigen_approx(true_values, 5e-2));
    } else if constexpr (Degree == 3) {
        CHECK_THAT(solution, eigen_approx(true_values, 5e-2));
    }
}
