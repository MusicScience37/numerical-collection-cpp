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
 * \brief Test to solve 2D diffusion equation with Dirichlet boundary condition.
 */
#include <cmath>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

#include "eigen_approx.h"
#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

TEST_CASE("Diffusion equation in 2D with Dirichlet boundary condition") {
    using position_type = Eigen::Vector2d;
    using solution_type = Eigen::VectorXd;
    using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    static constexpr double diffusion_coefficient = 0.1;
    constexpr double initial_time = 0.0;
    constexpr double final_time = 1.0;

    auto test_function = [](const position_type& position,
                             double time) -> double {
        return std::exp(-2.0 * diffusion_coefficient * num_collect::pi<double> *
                   num_collect::pi<double> * time) *
            (position.array() * num_collect::pi<double>).sin().prod();
    };

    num_collect::logging::logger logger;

    NUM_COLLECT_LOG_DEBUG(logger, "Generate nodes.");
#ifndef NDEBUG
    constexpr num_collect::index_type num_interior_nodes = 100;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 10;
#else
    constexpr num_collect::index_type num_interior_nodes = 10000;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 100;
#endif
    auto nodes =
        num_collect::rbf::generate_halton_nodes<typename position_type::Scalar,
            position_type::RowsAtCompileTime>(num_interior_nodes);
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    const auto interior_nodes =
        num_collect::util::vector_view<const position_type>(nodes).first(
            num_interior_nodes);

    NUM_COLLECT_LOG_DEBUG(logger, "Assemble the system.");
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<position_type>;
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    using ode_problem_type =
        num_collect::ode::problems::linear_first_order_ode_problem<
            solution_type, sparse_matrix_type>;
    assembler_type assembler;
    assembler.num_neighbors(15);
    num_collect::util::vector<Eigen::Triplet<double>> triplets;

    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    constexpr num_collect::index_type row_offset = 0;
    constexpr num_collect::index_type column_offset = 0;
    assembler.compute_rows(
        [](const position_type& position) {
            return diffusion_coefficient * operator_type(position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher,
        triplets, row_offset, column_offset);

    sparse_matrix_type whole_coefficients(num_interior_nodes, nodes.size());
    whole_coefficients.setFromTriplets(triplets.begin(), triplets.end());

    const sparse_matrix_type variable_coefficients =
        whole_coefficients.leftCols(num_interior_nodes);
    const solution_type constant_term = solution_type::Zero(num_interior_nodes);
    const ode_problem_type problem(variable_coefficients, constant_term);

    solution_type solution = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        solution(i) = test_function(nodes[i], initial_time);
    }

    NUM_COLLECT_LOG_DEBUG(logger, "Solve the system.");
    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_solver<ode_problem_type>;
    solver_type solver(problem);
    const solution_type initial_variable = solution.head(num_interior_nodes);
    solver.init(initial_time, initial_variable);
    solver.solve_till(final_time);
    solution.head(num_interior_nodes) = solver.variable();

    NUM_COLLECT_LOG_DEBUG(logger, "Evaluate the solution.");
    solution_type true_values = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        true_values(i) = test_function(nodes[i], final_time);
    }
    NUM_COLLECT_LOG_DEBUG(logger, "Max error: {}, mean error: {}",
        (solution - true_values).cwiseAbs().maxCoeff(),
        (solution - true_values).cwiseAbs().mean());
#ifndef NDEBUG
    CHECK_THAT(solution, eigen_approx(true_values, 1e-2));
#else
    CHECK_THAT(solution, eigen_approx(true_values, 1e-4));
#endif
}
