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
 * \brief Test of discretization of biharmonic operator.
 */
#include "num_collect/rbf/operators/biharmonic_operator.h"

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"

TEMPLATE_TEST_CASE_SIG(
    "Biharmonic operator in 2D with different polynomial degrees", "",
    ((int Degree), Degree), (0), (1), (2), (3), (4), (5)) {
    using position_type = Eigen::Vector2d;
    using solution_type = Eigen::VectorXd;
    using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    using operator_type =
        num_collect::rbf::operators::biharmonic_operator<position_type>;
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;

    auto test_function = [](const position_type& position) -> double {
        return (position.array() * num_collect::base::pi<double>).sin().prod();
    };
    auto test_function_biharmonic =
        [](const position_type& position) -> double {
        constexpr double coeff = 4.0 * num_collect::base::pi<double> *
            num_collect::base::pi<double> * num_collect::base::pi<double> *
            num_collect::base::pi<double>;
        return coeff *
            (position.array() * num_collect::base::pi<double>).sin().prod();
    };

    num_collect::logging::logger logger;

    NUM_COLLECT_LOG_DEBUG(logger, "Generate nodes.");
    constexpr num_collect::index_type num_interior_nodes = 100;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 10;
    auto nodes =
        num_collect::rbf::generate_halton_nodes<typename position_type::Scalar,
            position_type::RowsAtCompileTime>(num_interior_nodes);
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());

    constexpr num_collect::index_type num_test_nodes = 10;
    num_collect::util::vector<position_type> test_nodes;
    for (num_collect::index_type i = 0; i < num_test_nodes; ++i) {
        const double x = static_cast<double>(i + 1) / (num_test_nodes + 1);
        test_nodes.emplace_back(position_type::Constant(x));
    }

    NUM_COLLECT_LOG_DEBUG(logger, "Assemble the matrix.");
    assembler_type assembler(Degree);
    assembler.num_neighbors_at_least(15);
    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    const num_collect::util::nearest_neighbor_searcher<position_type>
        nearest_neighbor_searcher(nodes);
    assembler.compute_rows<operator_type>(
        test_nodes, nodes, nearest_neighbor_searcher, triplets, 0, 0);
    sparse_matrix_type matrix(test_nodes.size(), nodes.size());
    matrix.setFromTriplets(triplets.begin(), triplets.end());

    NUM_COLLECT_LOG_DEBUG(logger, "Compute the result.");
    const solution_type function_values =
        solution_type::NullaryExpr(nodes.size(),
            [&](num_collect::index_type i) { return test_function(nodes[i]); });
    const solution_type results = matrix * function_values;
    const solution_type expected_results = solution_type::NullaryExpr(
        test_nodes.size(), [&](num_collect::index_type i) {
            return test_function_biharmonic(test_nodes[i]);
        });

    constexpr num_collect::index_type precision = 4;
    comparison_approvals::verify_with_reference(
        results, expected_results, precision);
}
