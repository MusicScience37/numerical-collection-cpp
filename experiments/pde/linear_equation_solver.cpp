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
 * \brief Experiment of solvers of linear equations.
 */
#include <chrono>
#include <cmath>
#include <random>
#include <ratio>
#include <string>
#include <string_view>
#include <utility>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/functional_gmres.h"
#include "num_collect/linear/functional_preconditioned_gmres.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/operators/partial_derivative_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"
#include "toml_parser.h"

using position_type = Eigen::Vector2d;
using solution_type = Eigen::VectorXd;
using sparse_matrix_type = Eigen::SparseMatrix<double,
    Eigen::RowMajor>;  // BiCGstab works better with row-major format.

static auto test_function(const position_type& position, double time,
    double diffusion_coefficient) -> double {
    return std::exp(-2.0 * diffusion_coefficient * num_collect::pi<double> *
               num_collect::pi<double> * time) *
        (position.array() * num_collect::pi<double>).sin().prod();
}

/*!
 * \brief Generate nodes.
 *
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] num_boundary_nodes_per_edge Number of nodes on each edge of the
 * boundary.
 * \return Generated nodes. First num_interior_nodes nodes are in the interior,
 * and the rest are on the boundary in the counter-clockwise order.
 */
static auto generate_nodes(num_collect::index_type num_interior_nodes,
    num_collect::index_type num_boundary_nodes_per_edge)
    -> num_collect::util::vector<position_type> {
    auto interior_nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_interior_nodes);
    for (auto& node : interior_nodes) {
        node.y() *= 0.5;
    }
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 0.5),
            num_boundary_nodes_per_edge);
    num_collect::util::vector<position_type> nodes;
    nodes.reserve(interior_nodes.size() + boundary_nodes.size());
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    return nodes;
}

using ode_problem_type =
    num_collect::ode::problems::linear_first_order_dae_problem<solution_type,
        sparse_matrix_type>;

/*!
 * \brief Assemble the system of the diffusion equation.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] num_boundary_nodes_per_edge Number of nodes on each edge of the
 * boundary.
 * \param[in] polynomial_order Order of polynomials used in RBF-FD. -1 for no
 * polynomials.
 * \param[in] num_neighbors Number of neighbors used in RBF-FD.
 * \param[in] diffusion_coefficient Diffusion coefficient.
 * \return ODE problem to solve.
 */
static auto assemble_system(
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes,
    num_collect::index_type num_boundary_nodes_per_edge, int polynomial_order,
    num_collect::index_type num_neighbors, double diffusion_coefficient)
    -> ode_problem_type {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);

    num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
    stiffness_triplets.reserve(nodes.size() * num_neighbors);
    num_collect::util::vector<Eigen::Triplet<double>> mass_triplets;
    mass_triplets.reserve(nodes.size());
    Eigen::VectorXd load_vector = Eigen::VectorXd::Zero(nodes.size());

    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

    // Equations for interior nodes.
    {
        using operator_type =
            num_collect::rbf::operators::laplacian_operator<position_type>;
        const auto interior_nodes = nodes.first(num_interior_nodes);
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows(
            [diffusion_coefficient](const position_type& position) {
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
        const auto neumann_boundary_nodes = nodes.subview(
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

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(mass_matrix, stiffness_matrix, load_vector);
}

static auto create_linear_system(
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes,
    num_collect::index_type num_boundary_nodes_per_edge, int polynomial_order,
    num_collect::index_type num_neighbors, double diffusion_coefficient,
    double time_step_size) -> std::pair<sparse_matrix_type, solution_type> {
    auto ode_problem =
        assemble_system(nodes, num_interior_nodes, num_boundary_nodes_per_edge,
            polynomial_order, num_neighbors, diffusion_coefficient);

    double time = 0.0;
    solution_type solution = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        solution(i) = test_function(nodes[i], time, diffusion_coefficient);
    }
    ode_problem.evaluate_on(time, solution,
        num_collect::ode::evaluation_type{
            .diff_coeff = true, .jacobian = true, .mass = true});

    sparse_matrix_type coeff =
        ode_problem.mass() - time_step_size * ode_problem.jacobian();
    solution_type rhs = -ode_problem.diff_coeff();

    return std::make_pair(std::move(coeff), std::move(rhs));
}

static auto estimate_diag(const sparse_matrix_type& coeff,
    num_collect::index_type num_samples) -> solution_type {
    const num_collect::index_type size = coeff.cols();
    solution_type eval_vector;
    solution_type diag_estimate;
    eval_vector.resize(size);
    diag_estimate.resize(size);
    std::independent_bits_engine<std::mt19937, 1, unsigned> random_engine;
    for (num_collect::index_type sample = 0; sample < num_samples; ++sample) {
        for (num_collect::index_type i = 0; i < size; ++i) {
            eval_vector(i) = (random_engine() == 0) ? 1.0 : -1.0;
        }
        diag_estimate += (coeff * eval_vector).cwiseProduct(eval_vector);
    }
    diag_estimate /= static_cast<double>(num_samples);
    return diag_estimate;
}

static void solve_linear_system(const sparse_matrix_type& coeff,
    const solution_type& rhs, std::string_view solver_type,
    num_collect::index_type num_diag_estimation_samples) {
    num_collect::logging::logger logger;

    NUM_COLLECT_LOG_INFO(logger, "Start solving the linear system.");

    const auto start_time = std::chrono::steady_clock::now();

    constexpr double rel_tol = 1e-4;
    if (solver_type == "functional_gmres") {
        num_collect::linear::functional_gmres<solution_type> solver;
        solver.tolerance(rel_tol);
        solution_type solution = solution_type::Zero(rhs.size());
        solver.solve([&coeff](const solution_type& input,
                         solution_type& output) { output = coeff * input; },
            rhs, solution);
    } else if (solver_type == "functional_preconditioned_gmres") {
        num_collect::linear::functional_preconditioned_gmres<solution_type>
            solver;
        solver.tolerance(rel_tol);
        solver.prepare_preconditioner(coeff.diagonal());
        solution_type solution = solution_type::Zero(rhs.size());
        solver.solve([&coeff](const solution_type& input,
                         solution_type& output) { output = coeff * input; },
            rhs, solution);
    } else if (solver_type == "functional_preconditioned_gmres_approx") {
        num_collect::linear::functional_preconditioned_gmres<solution_type>
            solver;
        solver.tolerance(rel_tol);
        solver.prepare_preconditioner(
            estimate_diag(coeff, num_diag_estimation_samples));
        solution_type solution = solution_type::Zero(rhs.size());
        solver.solve([&coeff](const solution_type& input,
                         solution_type& output) { output = coeff * input; },
            rhs, solution);
    } else {
        NUM_COLLECT_LOG_ERROR(logger, "Unknown solver type: {}", solver_type);
    }

    const auto end_time = std::chrono::steady_clock::now();
    const auto elapsed_time_ms =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            end_time - start_time)
            .count();
    NUM_COLLECT_LOG_INFO(logger,
        "Finished solving the linear system with {:.5f} ms", elapsed_time_ms);
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/linear_equation_solver.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "linear_equation_solver.num_interior_nodes");
    const auto num_boundary_nodes_per_edge =
        parser.get<num_collect::index_type>(
            "linear_equation_solver.num_boundary_nodes_per_edge");
    const auto polynomial_order =
        parser.get<int>("linear_equation_solver.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "linear_equation_solver.num_neighbors");
    const auto diffusion_coefficient =
        parser.get<double>("linear_equation_solver.diffusion_coefficient");
    const auto time_step_size =
        parser.get<double>("linear_equation_solver.time_step_size");
    const auto solver_type =
        parser.get<std::string>("linear_equation_solver.solver_type");
    const auto num_diag_estimation_samples =
        parser.get<num_collect::index_type>(
            "linear_equation_solver.num_diag_estimation_samples");
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Number of boundary nodes per edge: {}",
        num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(
        logger, "Diffusion coefficient: {}", diffusion_coefficient);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(logger, "Solver type: {}", solver_type);
    NUM_COLLECT_LOG_INFO(logger,
        "Number of samples for diagonal estimation: {}",
        num_diag_estimation_samples);

    const auto nodes =
        generate_nodes(num_interior_nodes, num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    const auto [coeff, rhs] = create_linear_system(nodes, num_interior_nodes,
        num_boundary_nodes_per_edge, polynomial_order, num_neighbors,
        diffusion_coefficient, time_step_size);

    solve_linear_system(coeff, rhs, solver_type, num_diag_estimation_samples);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
