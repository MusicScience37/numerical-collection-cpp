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
 * \brief Test to solve 2D diffusion equation with Neumann boundary conditions.
 */
#include <cmath>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

#include "eigen_approx.h"
#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/operators/partial_derivative_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

TEST_CASE("Diffusion equation in 2D with Neumann boundary conditions") {
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
    // Many nodes results in too long time even in release mode.
    constexpr num_collect::index_type num_interior_nodes = 400;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 20;
#endif
    auto nodes =
        num_collect::rbf::generate_halton_nodes<typename position_type::Scalar,
            position_type::RowsAtCompileTime>(num_interior_nodes);
    for (auto& node : nodes) {
        node.y() *= 0.5;
    }
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 0.5),
            num_boundary_nodes_per_edge);
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    const auto interior_nodes =
        num_collect::util::vector_view<const position_type>(nodes).first(
            num_interior_nodes);

    NUM_COLLECT_LOG_DEBUG(logger, "Assemble the system.");

    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);

    constexpr num_collect::index_type num_neighbors = 15;
    num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
    stiffness_triplets.reserve(nodes.size() * num_neighbors);
    num_collect::util::vector<Eigen::Triplet<double>> mass_triplets;
    mass_triplets.reserve(nodes.size());
    Eigen::VectorXd load_vector = Eigen::VectorXd::Zero(nodes.size());

    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler;
    assembler.num_neighbors(num_neighbors);

    // Equations for interior nodes.
    {
        using operator_type =
            num_collect::rbf::operators::laplacian_operator<position_type>;
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows(
            [](const position_type& position) {
                return diffusion_coefficient * operator_type(position);
            },
            interior_nodes, nodes, column_variables_nearest_neighbor_searcher,
            stiffness_triplets, row_offset, column_offset);
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            mass_triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), 1.0);
        }
        // Elements of the load vector are left as zero.
    }

    // Equations for Neumann boundary nodes. (End points are excluded.)
    const num_collect::index_type first_neumann_boundary_node_index =
        num_interior_nodes + 2 * num_boundary_nodes_per_edge + 1;
    const num_collect::index_type last_neumann_boundary_node_index =
        num_interior_nodes + 3 * num_boundary_nodes_per_edge - 1;
    {
        using operator_type =
            num_collect::rbf::operators::partial_derivative_operator<
                position_type, 1>;
        const num_collect::index_type num_neumann_boundary_nodes =
            last_neumann_boundary_node_index -
            first_neumann_boundary_node_index + 1;
        const auto neumann_boundary_nodes =
            num_collect::util::vector_view<const position_type>(nodes).subview(
                first_neumann_boundary_node_index, num_neumann_boundary_nodes);
        const num_collect::index_type row_offset =
            first_neumann_boundary_node_index;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows<operator_type>(neumann_boundary_nodes, nodes,
            column_variables_nearest_neighbor_searcher, stiffness_triplets,
            row_offset, column_offset);
        // Elements of the mass matrix and the load vector are left as zero.
    }

    // Equations for Dirichlet boundary nodes.
    for (num_collect::index_type i = num_interior_nodes; i < nodes.size();
        ++i) {
        const bool is_neumann_boundary =
            (i >= first_neumann_boundary_node_index) &&
            (i <= last_neumann_boundary_node_index);
        if (!is_neumann_boundary) {
            stiffness_triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), 1.0);
            // Elements of the mass matrix and the load vector are left as zero.
        }
    }

    sparse_matrix_type stiffness_matrix(nodes.size(), nodes.size());
    stiffness_matrix.setFromTriplets(
        stiffness_triplets.begin(), stiffness_triplets.end());
    sparse_matrix_type mass_matrix(nodes.size(), nodes.size());
    mass_matrix.setFromTriplets(mass_triplets.begin(), mass_triplets.end());

    using ode_problem_type =
        num_collect::ode::problems::linear_first_order_dae_problem<
            solution_type, sparse_matrix_type>;
    const ode_problem_type ode_problem(
        mass_matrix, stiffness_matrix, load_vector);

    solution_type solution = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        solution(i) = test_function(nodes[i], initial_time);
    }

    NUM_COLLECT_LOG_DEBUG(logger, "Solve the system.");
    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_solver<ode_problem_type>;
    solver_type solver(ode_problem);
    solver.init(initial_time, solution);
    solver.solve_till(final_time);
    solution = solver.variable();

    NUM_COLLECT_LOG_DEBUG(logger, "Evaluate the solution.");
    solution_type true_values = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        true_values(i) = test_function(nodes[i], final_time);
    }
    NUM_COLLECT_LOG_DEBUG(logger, "Max error: {}, mean error: {}",
        (solution - true_values).cwiseAbs().maxCoeff(),
        (solution - true_values).cwiseAbs().mean());
#ifndef NDEBUG
    CHECK_THAT(solution, eigen_approx(true_values, 1e-2));
#else
    CHECK_THAT(solution, eigen_approx(true_values, 5e-3));
#endif
}
