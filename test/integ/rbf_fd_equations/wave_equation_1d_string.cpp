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
 * \brief Test to solve 1D wave equation of a string using RBF-FD method.
 */
#include <algorithm>
#include <cmath>
#include <string_view>

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
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

TEST_CASE("Wave equation of a string in 1D") {
    using position_type = double;
    using solution_type = Eigen::VectorXd;
    using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    static constexpr double wave_speed = 1.0;
    constexpr double initial_time = 0.0;
    constexpr double final_time = 1.0;

    auto test_function = [](const position_type& position,
                             double time) -> double {
        return std::sin(num_collect::pi<double> * position) *
            std::cos(wave_speed * num_collect::pi<double> * time);
    };

    num_collect::logging::logger logger;

    NUM_COLLECT_LOG_DEBUG(logger, "Generate nodes.");
    constexpr num_collect::index_type num_interior_nodes = 100;
    auto nodes =
        num_collect::rbf::generate_1d_halton_nodes<double>(num_interior_nodes);
    std::ranges::sort(nodes);
    nodes.push_back(0.0);
    nodes.push_back(1.0);
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
    // Variables of ODE problem are [du/dt^\top, u^\top]^\top.
    constexpr int polynomial_order = 4;
    constexpr num_collect::index_type num_neighbors = 20;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);
    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    const double squared_wave_speed = wave_speed * wave_speed;
    num_collect::util::vector<Eigen::Triplet<double>> laplacian_triplets;
    assembler.compute_rows(
        [squared_wave_speed](const position_type& position) {
            return squared_wave_speed * operator_type(position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher,
        laplacian_triplets);

    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    // Top right block.
    for (const auto& triplet : laplacian_triplets) {
        if (triplet.col() < num_interior_nodes) {
            triplets.emplace_back(triplet.row(),
                triplet.col() + static_cast<int>(num_interior_nodes),
                triplet.value());
        }
    }
    // Bottom left block.
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        triplets.emplace_back(
            static_cast<int>(i + num_interior_nodes), static_cast<int>(i), 1.0);
    }
    sparse_matrix_type coefficients(
        2 * num_interior_nodes, 2 * num_interior_nodes);
    coefficients.setFromTriplets(triplets.begin(), triplets.end());

    const solution_type constant_term =
        solution_type::Zero(2 * num_interior_nodes);

    const ode_problem_type problem(coefficients, constant_term);

    NUM_COLLECT_LOG_DEBUG(logger, "Solve the system.");

    solution_type variable = solution_type::Zero(2 * num_interior_nodes);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        variable(i + num_interior_nodes) =
            test_function(nodes[i], initial_time);
    }

    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_adaptive_step_solver<
            ode_problem_type>;
    solver_type solver(problem);
    solver.init(initial_time, variable);
    solver.solve_until(final_time);
    const solution_type solution = solver.variable().tail(num_interior_nodes);

    NUM_COLLECT_LOG_DEBUG(logger, "Evaluate the solution.");
    solution_type true_values = solution_type::Zero(num_interior_nodes);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        true_values(i) = test_function(nodes[i], final_time);
    }
    NUM_COLLECT_LOG_DEBUG(logger, "Max error: {}, mean error: {}",
        (solution - true_values).cwiseAbs().maxCoeff(),
        (solution - true_values).cwiseAbs().mean());
    CHECK_THAT(solution, eigen_approx(true_values, 1e-5));
}
