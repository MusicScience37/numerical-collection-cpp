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

// Example: Solving 2D diffusion equation with Dirichlet boundary conditions
// using RBF-FD method.
//
// This example demonstrates how to solve the following 2D diffusion equation:
//   ∂u/∂t = α(∂²u/∂x² + ∂²u/∂y²)  (0 < x < 1, 0 < y < 1, t > 0)
//
// Boundary conditions:
//   u(x, y, t) = 0  on all boundaries (Dirichlet)
//
// Initial condition:
//   u(x, y, 0) = sin(πx) sin(πy)
//
// Analytical solution:
//   u(x, y, t) = exp(-2απ²t) sin(πx) sin(πy)
//
// Solution method:
//   - Spatial discretization: RBF-FD (Radial Basis Function - Finite
//     Difference) with quasi-random Halton nodes
//   - Time integration: Rosenbrock method (RODASPR) for the first-order ODE
//     system
//
// Output:
//   - Errors are logged at each time step
//   - Time evolution is visualized and saved as
//     rbf_fd_diffusion_equation_2d_dirichlet.pvd and
//     rbf_fd_diffusion_equation_2d_dirichlet_XXXX.vtp files for ParaView
#include <cmath>
#include <filesystem>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/format.h>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
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
#include "para_view_time_steps_data_file_writer.h"
#include "toml_parser.h"
#include "write_vtp_file_for_comparison.h"

// Type definitions for better readability.
// Node positions in 2D space.
using position_type = Eigen::Vector2d;
// Solution vectors (values at interior nodes).
using solution_type = Eigen::VectorXd;
// Sparse matrix (row-major for performance).
using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

// ODE problem type for the first-order system.
// The diffusion equation ∂u/∂t = α∇²u is a first-order ODE:
//   du/dt = α∇²u
// ODE variable: u at interior nodes only (boundary values are always zero).
using ode_problem_type =
    num_collect::ode::problems::linear_first_order_ode_problem<solution_type,
        sparse_matrix_type>;

// Directory for output ParaView visualization files.
static constexpr std::string_view output_directory =
    "rbf_fd_diffusion_equation_2d_dirichlet";

// Analytical solution: u(x, y, t) = exp(-2απ²t) sin(πx) sin(πy).
// Used for initial conditions and error evaluation.
static auto test_function(const position_type& position, double time,
    double diffusion_coefficient) -> double {
    return std::exp(-2.0 * diffusion_coefficient * num_collect::pi<double> *
               num_collect::pi<double> * time) *
        (position.array() * num_collect::pi<double>).sin().prod();
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

// Assemble the linear system for the diffusion equation ODE.
//
// This function constructs the coefficient matrix for the first-order ODE
// system. The RBF-FD method is used to approximate the Laplacian operator
// (∂²/∂x² + ∂²/∂y²).
//
// Parameters:
//   nodes: All nodes (interior + boundary), where first num_interior_nodes are
//     interior nodes
//   num_interior_nodes: Number of interior nodes
//   polynomial_order: Order of polynomials in RBF-FD
//   num_neighbors: Number of neighboring nodes used in each RBF-FD stencil
//   diffusion_coefficient: Diffusion coefficient α in the equation
//
// Returns:
//   Linear first-order ODE problem: du/dt = A*u, where u is the solution at
//   interior nodes only
static auto assemble_system(
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double diffusion_coefficient)
    -> ode_problem_type {
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<position_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    num_collect::util::vector<Eigen::Triplet<double>> triplets;

    // Set up RBF-FD assembler with PHS (Polyharmonic Spline) + polynomial
    // augmentation.
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

    // Compute Laplacian matrix for interior nodes.
    const auto interior_nodes = nodes.first(num_interior_nodes);
    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    constexpr num_collect::index_type row_offset = 0;
    constexpr num_collect::index_type column_offset = 0;
    assembler.compute_rows(
        [diffusion_coefficient](const position_type& position) {
            return diffusion_coefficient * operator_type(position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher,
        triplets, row_offset, column_offset);

    sparse_matrix_type whole_coefficients(num_interior_nodes, nodes.size());
    whole_coefficients.setFromTriplets(triplets.begin(), triplets.end());

    // Use only the part corresponding to interior nodes for the ODE system
    // to express the Dirichlet boundary conditions (u = 0 on the boundary)
    // implicitly.
    const sparse_matrix_type variable_coefficients =
        whole_coefficients.leftCols(num_interior_nodes);
    const solution_type constant_term = solution_type::Zero(num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(variable_coefficients, constant_term);
}

// Solve the diffusion equation ODE system and visualize results.
//
// This function integrates the first-order ODE system in time using the RODASPR
// method (a Rosenbrock-type solver). At each time step, it compares the
// numerical solution with the analytical solution, logs the errors,
// and saves the results in ParaView format (.vtp files).
//
// Parameters:
//   problem: Linear ODE problem assembled by assemble_system
//   diffusion_coefficient: Diffusion coefficient α
//   time_step_size: Time step for output (solver uses adaptive stepping
//     internally)
//   final_time: End time of simulation
//   nodes: All nodes (same as in assemble_system)
//   num_interior_nodes: Number of interior nodes
//
// Output:
//   - Logs error statistics at each time step
//   - Saves visualization to rbf_fd_diffusion_equation_2d_dirichlet/ directory
//     in ParaView format (.pvd and .vtp files)
static void solve_system(const ode_problem_type& problem,
    double diffusion_coefficient, double time_step_size, double final_time,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;

    std::filesystem::create_directories(output_directory);

    const std::string para_view_data_file_path = fmt::format(
        "{}/rbf_fd_diffusion_equation_2d_dirichlet.pvd", output_directory);
    para_view_time_steps_data_file_writer para_view_writer(
        para_view_data_file_path);

    // Initialize time and solution vector.
    num_collect::index_type time_index = 0;
    double time = 0.0;

    // Set initial condition: u(x,y,0) = sin(πx) sin(πy), boundary values = 0.
    solution_type whole_variable = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        whole_variable(i) =
            test_function(nodes[i], time, diffusion_coefficient);
    }
    solution_type true_values = whole_variable;
    solution_type errors = solution_type::Zero(nodes.size());
    std::string vtp_file_name = fmt::format(
        "rbf_fd_diffusion_equation_2d_dirichlet_{:04d}.vtp", time_index);
    write_vtp_file_for_comparison(
        fmt::format("{}/{}", output_directory, vtp_file_name), nodes,
        whole_variable, true_values, errors);
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

        // Compute analytical solution at current time.
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            true_values(i) =
                test_function(nodes[i], time, diffusion_coefficient);
        }

        // Calculate errors.
        errors = (whole_variable - true_values).cwiseAbs();
        const double max_error = errors.maxCoeff();
        const double mean_error = errors.head(num_interior_nodes).mean();
        const double max_value = true_values.cwiseAbs().maxCoeff();
        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
            "{:.2e}",
            time, max_error, mean_error, max_value);

        // Add current solution to visualization.
        vtp_file_name = fmt::format(
            "rbf_fd_diffusion_equation_2d_dirichlet_{:04d}.vtp", time_index);
        write_vtp_file_for_comparison(
            fmt::format("{}/{}", output_directory, vtp_file_name), nodes,
            whole_variable, true_values, errors);
        para_view_writer.add_time_step(time, vtp_file_name);
    }
}

auto main(int argc, const char** argv) -> int {
    // Load configuration file.
    std::string_view config_file_path =
        "examples/pde/rbf_fd_diffusion_equation_2d_dirichlet.toml";
    if (argc == 2) {
        config_file_path = argv[1];  // Use command-line argument if provided.
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    // Parse simulation parameters from TOML configuration file.
    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "rbf_fd_diffusion_equation_2d_dirichlet.num_interior_nodes");
    const auto num_boundary_nodes_per_edge = parser.get<
        num_collect::index_type>(
        "rbf_fd_diffusion_equation_2d_dirichlet.num_boundary_nodes_per_edge");
    const auto polynomial_order = parser.get<int>(
        "rbf_fd_diffusion_equation_2d_dirichlet.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "rbf_fd_diffusion_equation_2d_dirichlet.num_neighbors");
    const auto diffusion_coefficient = parser.get<double>(
        "rbf_fd_diffusion_equation_2d_dirichlet.diffusion_coefficient");
    const auto time_step_size = parser.get<double>(
        "rbf_fd_diffusion_equation_2d_dirichlet.time_step_size");
    const auto final_time =
        parser.get<double>("rbf_fd_diffusion_equation_2d_dirichlet.final_time");

    // Log configuration parameters.
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Number of boundary nodes per edge: {}",
        num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(
        logger, "Diffusion coefficient: {}", diffusion_coefficient);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(logger, "Final time: {}", final_time);

    // Generate spatial discretization nodes.
    const auto nodes =
        generate_nodes(num_interior_nodes, num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    // Assemble the ODE system matrix.
    const auto problem = assemble_system(nodes, num_interior_nodes,
        polynomial_order, num_neighbors, diffusion_coefficient);
    // Solve the ODE system and visualize.
    solve_system(problem, diffusion_coefficient, time_step_size, final_time,
        nodes, num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
