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

// Example: Solving 2D advection diffusion equation using RBF-FD method.

// TODO further description.

#include <cmath>
#include <filesystem>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/format.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/operators/partial_derivative_operator.h"
#include "num_collect/rbf/operators/polyharmonic_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/eigen_triplets_util.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"
#include "para_view_time_steps_data_file_writer.h"
#include "toml_parser.h"
#include "write_vtp_file_with_solution.h"

// Type definitions for better readability.
// Node positions in 2D space.
using position_type = Eigen::Vector2d;
// Solution vectors (values at interior nodes).
using solution_type = Eigen::VectorXd;
// Sparse matrix (row-major for performance).
using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

// ODE problem type for the first-order system.
using ode_problem_type =
    num_collect::ode::problems::linear_first_order_ode_problem<solution_type,
        sparse_matrix_type>;

// Directory for output ParaView visualization files.
static constexpr std::string_view output_directory =
    "rbf_fd_advection_diffusion_equation_2d";

// Initial solution.
// This function is smooth and compactly supported.
static auto test_function(const position_type& position) -> double {
    constexpr double center_x = 0.2;
    constexpr double center_y = 0.5;
    constexpr double radius = 0.1;
    constexpr double squared_radius = radius * radius;
    const double relative_x = position.x() - center_x;
    const double relative_y = position.y() - center_y;
    const double squared_distance =
        relative_x * relative_x + relative_y * relative_y;
    if (squared_distance >= squared_radius) {
        return 0.0;
    }
    return std::exp(1.0 - 1.0 / (1.0 - squared_distance / squared_radius));
}

// Generate spatial discretization nodes.
//
// Creates a set of nodes in the square domain [0, 1]²:
//   - Interior nodes: Generated using Halton sequence
//   - Boundary nodes: Distributed on all four edges of the square
//
// Parameters:
//   num_interior_nodes: Number of nodes in the interior
//   num_boundary_nodes_per_edge: Number of nodes on each edge of the boundary
//
// Returns:
//   Vector of nodes where first num_interior_nodes are interior nodes,
//   followed by boundary nodes in counter-clockwise order
static auto generate_nodes(num_collect::index_type num_interior_nodes,
    num_collect::index_type num_boundary_nodes_per_edge)
    -> num_collect::util::vector<position_type> {
    // Generate interior nodes using Halton sequence.
    const auto interior_nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_interior_nodes);
    // Generate boundary nodes on all four edges.
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    // Combine interior and boundary nodes.
    num_collect::util::vector<position_type> nodes;
    nodes.reserve(interior_nodes.size() + boundary_nodes.size());
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    return nodes;
}

// Assemble the linear system for the advection diffusion equation ODE.
static auto assemble_system(
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double advection_velocity,
    double diffusion_coefficient, double hyperviscosity_rate)
    -> ode_problem_type {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    // Set up RBF-FD assembler with PHS (Polyharmonic Spline) + polynomial
    // augmentation.
    constexpr int phs_degree = 7;
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type,
            phs_degree>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

    static constexpr int hyperviscosity_order = 3;
    const double discretization_width =
        1.0 / std::sqrt(static_cast<double>(num_interior_nodes));
    // Hyperviscosity coefficient: (-1)^(p-1) ε h^(2p), scaled to the mesh
    // width h = 1/N so the stabilization is proportional to the resolution.
    const double hyperviscosity_coeff =
        std::pow(-1.0, hyperviscosity_order - 1) * hyperviscosity_rate *
        std::pow(discretization_width, 2 * hyperviscosity_order);

    // Compute stiffness matrix for interior nodes.
    const auto interior_nodes = nodes.first(num_interior_nodes);
    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    const auto triplets = assembler.compute_rows(
        [advection_velocity, diffusion_coefficient, hyperviscosity_coeff](
            const position_type& position) {
            return
                // Advection term.
                -advection_velocity *
                num_collect::rbf::operators::partial_derivative_operator<
                    position_type, 0>(position)
                // Hyperviscosity term for stabilization.
                + hyperviscosity_coeff *
                num_collect::rbf::operators::polyharmonic_operator<
                    hyperviscosity_order, position_type>(position)
                // Diffusion term.
                + diffusion_coefficient *
                num_collect::rbf::operators::laplacian_operator<position_type>(
                    position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher);

    // Use only the part corresponding to interior nodes for the ODE system
    // to express the Dirichlet boundary conditions (u = 0 on the boundary)
    // implicitly.
    using num_collect::util::eigen_triplets::filter_columns;
    using num_collect::util::eigen_triplets::to_sparse_matrix;
    const auto variable_coefficients = triplets |
        filter_columns(0, num_interior_nodes) |
        to_sparse_matrix<sparse_matrix_type>(
            num_interior_nodes, num_interior_nodes);
    const solution_type constant_term = solution_type::Zero(num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(variable_coefficients, constant_term);
}

static void solve_system(const ode_problem_type& problem, double time_step_size,
    double final_time,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;

    std::filesystem::create_directories(output_directory);

    const std::string para_view_data_file_path = fmt::format(
        "{}/rbf_fd_advection_diffusion_equation_2d.pvd", output_directory);
    para_view_time_steps_data_file_writer para_view_writer(
        para_view_data_file_path);

    // Initialize time and solution vector.
    num_collect::index_type time_index = 0;
    double time = 0.0;

    solution_type whole_variable = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        whole_variable(i) = test_function(nodes[i]);
    }
    std::string vtp_file_name = fmt::format(
        "rbf_fd_advection_diffusion_equation_2d_{:04d}.vtp", time_index);
    write_vtp_file_with_solution(
        fmt::format("{}/{}", output_directory, vtp_file_name), nodes,
        whole_variable);
    para_view_writer.add_time_step(time, vtp_file_name);

    // Set up ODE solver.
    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_adaptive_step_solver<
            ode_problem_type>;
    solver_type solver(problem);
    const solution_type initial_variable =
        whole_variable.head(num_interior_nodes);
    solver.init(time, initial_variable);

    // Time integration loop.
    while (time < final_time) {
        // Determine next output time.
        double next_time = time + time_step_size;
        constexpr double time_threshold = 1e-4;
        if (next_time > final_time - time_threshold) {
            next_time = final_time;
        }

        // Integrate to next_time (solver uses adaptive internal steps).
        solver.solve_until(next_time);

        time = solver.time();
        ++time_index;
        whole_variable.head(num_interior_nodes) = solver.variable();

        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max value: {:.2e}, Min value: {:.2e}", time,
            whole_variable.maxCoeff(), whole_variable.minCoeff());

        // Add current solution to visualization.
        vtp_file_name = fmt::format(
            "rbf_fd_advection_diffusion_equation_2d_{:04d}.vtp", time_index);
        write_vtp_file_with_solution(
            fmt::format("{}/{}", output_directory, vtp_file_name), nodes,
            whole_variable);
        para_view_writer.add_time_step(time, vtp_file_name);
    }
}

auto main(int argc, const char** argv) -> int {
    // Load configuration file.
    std::string_view config_file_path =
        "examples/pde/rbf_fd_advection_diffusion_equation_2d.toml";
    if (argc == 2) {
        config_file_path = argv[1];  // Use command-line argument if provided.
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    // Parse simulation parameters from TOML configuration file.
    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "rbf_fd_advection_diffusion_equation_2d.num_interior_nodes");
    const auto num_boundary_nodes_per_edge = parser.get<
        num_collect::index_type>(
        "rbf_fd_advection_diffusion_equation_2d.num_boundary_nodes_per_edge");
    const auto polynomial_order = parser.get<int>(
        "rbf_fd_advection_diffusion_equation_2d.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "rbf_fd_advection_diffusion_equation_2d.num_neighbors");
    const auto advection_velocity = parser.get<double>(
        "rbf_fd_advection_diffusion_equation_2d.advection_velocity");
    const auto diffusion_coefficient = parser.get<double>(
        "rbf_fd_advection_diffusion_equation_2d.diffusion_coefficient");
    const auto hyperviscosity_rate = parser.get<double>(
        "rbf_fd_advection_diffusion_equation_2d.hyperviscosity_rate");
    const auto time_step_size = parser.get<double>(
        "rbf_fd_advection_diffusion_equation_2d.time_step_size");
    const auto final_time =
        parser.get<double>("rbf_fd_advection_diffusion_equation_2d.final_time");

    // Log configuration parameters.
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Number of boundary nodes per edge: {}",
        num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(logger, "Advection velocity: {}", advection_velocity);
    NUM_COLLECT_LOG_INFO(
        logger, "Diffusion coefficient: {}", diffusion_coefficient);
    NUM_COLLECT_LOG_INFO(
        logger, "Hyperviscosity rate: {}", hyperviscosity_rate);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(logger, "Final time: {}", final_time);

    // Generate spatial discretization nodes.
    const auto nodes =
        generate_nodes(num_interior_nodes, num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    // Assemble the ODE system matrix.
    const auto problem = assemble_system(nodes, num_interior_nodes,
        polynomial_order, num_neighbors, advection_velocity,
        diffusion_coefficient, hyperviscosity_rate);

    // Solve the ODE system and visualize.
    solve_system(
        problem, time_step_size, final_time, nodes, num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
