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

// Example: Solving 1D wave equation on a string using RBF-FD method
//
// This example demonstrates how to solve the following 1D wave equation:
//   ∂²u/∂t² = c² ∂²u/∂x²  (0 < x < 1, t > 0)
//
// Boundary conditions:
//   u(0, t) = 0, u(1, t) = 0  (fixed at both ends)
//
// Initial conditions:
//   u(x, 0) = sin(πx)         (initial displacement)
//   ∂u/∂t(x, 0) = 0           (initial velocity is zero)
//
// Analytical solution:
//   u(x, t) = sin(πx) cos(cπt)
//
// Solution method:
//   - Spatial discretization: RBF-FD (Radial Basis Function - Finite
//     Difference) with quasi-random Halton nodes
//   - Time integration: Rosenbrock method (RODASPR) for the first-order ODE
//     system
//   - The second-order wave equation is converted to a first-order system:
//     [∂u/∂t, u] = [c²∇²u, ∂u/∂t]
//
// Output:
//   - Errors are logged at each time step
//   - Time evolution is visualized and saved as
//     rbf_fd_wave_equation_1d_string.html
#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/format.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"
#include "toml_parser.h"

// Type definitions for better readability
// Node positions (x-coordinates)
using position_type = double;
// Solution vectors
using solution_type = Eigen::VectorXd;
// Sparse matrix (row-major for performance)
using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

// ODE problem type for the first-order system
// The wave equation ∂²u/∂t² = c²∇²u is converted to a first-order system:
//   d/dt [∂u/∂t] = [c²∇²u    ]
//        [u    ]   [∂u/∂t   ]
// ODE variable structure (size 2*num_interior_nodes):
//   variable[0 .. num_interior_nodes-1]: ∂u/∂t at interior nodes
//   variable[num_interior_nodes .. 2*num_interior_nodes-1]: u at interior nodes
using ode_problem_type =
    num_collect::ode::problems::linear_first_order_ode_problem<solution_type,
        sparse_matrix_type>;

// Analytical solution: u(x, t) = sin(πx) cos(cπt)
// Used for initial conditions and error evaluation
static auto test_function(
    const position_type& position, double time, double wave_speed) -> double {
    return std::sin(num_collect::pi<double> * position) *
        std::cos(wave_speed * num_collect::pi<double> * time);
}

// Generate spatial discretization nodes
//
// Creates a set of nodes in the domain [0, 1]:
//   - Interior nodes: Generated using Halton sequence
//   - Boundary nodes: Added at x=0 and x=1
//
// Parameters:
//   num_interior_nodes: Number of nodes in the interior (0 < x < 1)
//
// Returns:
//   Vector of nodes where:
//     - nodes[0] to nodes[num_interior_nodes-1]: interior nodes (sorted)
//     - nodes[num_interior_nodes]: boundary node at x=0
//     - nodes[num_interior_nodes+1]: boundary node at x=1
static auto generate_nodes(num_collect::index_type num_interior_nodes)
    -> num_collect::util::vector<position_type> {
    // Generate interior nodes using Halton sequence
    auto interior_nodes =
        num_collect::rbf::generate_1d_halton_nodes<double>(num_interior_nodes);
    std::ranges::sort(interior_nodes);

    // Combine interior and boundary nodes
    num_collect::util::vector<position_type> nodes;
    nodes.reserve(interior_nodes.size() + 2);
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.push_back(0.0);  // Left boundary
    nodes.push_back(1.0);  // Right boundary
    return nodes;
}

// Assemble the linear system for the wave equation ODE
//
// This function constructs the coefficient matrix for the first-order ODE
// system. The RBF-FD method is used to approximate the Laplacian operator
// (∂²/∂x²).
//
// Parameters:
//   nodes: All nodes (interior + boundary), where first num_interior_nodes are
//     interior nodes
//   num_interior_nodes: Number of interior nodes
//   polynomial_order: Order of polynomial augmentation in RBF-FD
//   num_neighbors: Number of neighboring nodes used in each RBF-FD stencil
//   wave_speed: Wave propagation speed (c in the equation)
//
// Returns:
//   Linear first-order ODE problem: dv/dt = Av + b, where v = [∂u/∂t, u]ᵀ
static auto assemble_system(
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double wave_speed)
    -> ode_problem_type {
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<position_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    num_collect::util::vector<Eigen::Triplet<double>> laplacian_triplets;

    // Set up RBF-FD assembler with PHS (Polyharmonic Spline) + polynomial
    // augmentation
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

    // Compute Laplacian matrix for interior nodes
    const auto interior_nodes = nodes.first(num_interior_nodes);
    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    constexpr num_collect::index_type row_offset = 0;
    constexpr num_collect::index_type column_offset = 0;
    const double squared_wave_speed = wave_speed * wave_speed;
    assembler.compute_rows(
        [squared_wave_speed](const position_type& position) {
            return squared_wave_speed * operator_type(position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher,
        laplacian_triplets, row_offset, column_offset);

    // Build the coefficient matrix for the first-order ODE system
    // The matrix has the following block structure:
    //   [  0      c²∇²  ]  (top row: d(∂u/∂t)/dt = c²∇²u)
    //   [  I       0    ]  (bottom row: du/dt = ∂u/∂t)
    num_collect::util::vector<Eigen::Triplet<double>> triplets;

    // Top right block: c²∇² applied to u variables
    for (const auto& triplet : laplacian_triplets) {
        if (triplet.col() < num_interior_nodes) {
            triplets.emplace_back(triplet.row(),
                triplet.col() + static_cast<int>(num_interior_nodes),
                triplet.value());
        }
    }

    // Bottom left block: Identity matrix for du/dt = ∂u/∂t
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        triplets.emplace_back(
            static_cast<int>(i + num_interior_nodes), static_cast<int>(i), 1.0);
    }

    // Create sparse matrix
    sparse_matrix_type coefficients(
        2 * num_interior_nodes, 2 * num_interior_nodes);
    coefficients.setFromTriplets(triplets.begin(), triplets.end());

    // Constant term is zero (homogeneous boundary conditions: u(0,t) = u(1,t) =
    // 0)
    const solution_type constant_term =
        solution_type::Zero(2 * num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(coefficients, constant_term);
}

// Solve the wave equation ODE system and visualize results
//
// This function integrates the first-order ODE system in time using the RODASPR
// method (a Rosenbrock-type solver). At each time step, it compares the
// numerical solution with the analytical solution and logs the errors.
//
// Parameters:
//   problem: Linear ODE problem assembled by assemble_system
//   wave_speed: Wave propagation speed
//   time_step_size: Time step for output (solver uses adaptive stepping
//   internally)
//   final_time: End time of simulation
//   nodes: All nodes (same as in assemble_system)
//   num_interior_nodes: Number of interior nodes
//
// Output:
//   - Logs error statistics at each time step
//   - Saves visualization to rbf_fd_wave_equation_1d_string.html
static void solve_system(const ode_problem_type& problem, double wave_speed,
    double time_step_size, double final_time,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start to solve the system.");

    // Initialize time and solution vector
    double time = 0.0;
    solution_type variable = solution_type::Zero(2 * num_interior_nodes);

    // Set initial condition: u(x,0) = sin(πx), ∂u/∂t(x,0) = 0
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        variable(i + num_interior_nodes) =  // u at interior nodes
            test_function(nodes[i], time, wave_speed);
        // variable(i) = 0 (∂u/∂t = 0 initially)
    }

    // Variables for error evaluation
    solution_type true_values = variable.tail(num_interior_nodes);
    solution_type errors = solution_type::Zero(num_interior_nodes);

    // Prepare nodes for visualization (including boundary points)
    num_collect::util::vector<position_type> visualized_nodes;
    visualized_nodes.reserve(num_interior_nodes + 2);
    visualized_nodes.push_back(0.0);  // Left boundary
    visualized_nodes.insert(visualized_nodes.end(), nodes.begin(),
        nodes.begin() + num_interior_nodes);  // Interior nodes
    visualized_nodes.push_back(1.0);          // Right boundary

    // Initialize visualization
    solution_type visualized_solution =
        solution_type::Zero(visualized_nodes.size());
    visualized_solution.segment(1, num_interior_nodes) =
        variable.tail(num_interior_nodes);  // u values (boundaries are 0)

    plotly_plotter::figure figure;
    auto scatter = figure.add_scatter();
    scatter.x(visualized_nodes);
    scatter.y(visualized_solution);
    scatter.mode("lines");
    scatter.name(fmt::format("t = {:.3f}", time));

    // Set up ODE solver
    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_adaptive_step_solver<
            ode_problem_type>;
    solver_type solver(problem);
    solver.init(time, variable);

    // Time integration loop
    while (time < final_time) {
        // Determine next output time
        double next_time = time + time_step_size;
        constexpr double time_threshold = 1e-4;
        if (next_time > final_time - time_threshold) {
            next_time = final_time;
        }

        // Integrate to next_time (solver uses adaptive internal steps)
        solver.solve_until(next_time);

        time = solver.time();
        variable = solver.variable();

        // Compute analytical solution at current time
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            true_values(i) = test_function(nodes[i], time, wave_speed);
        }

        // Calculate errors
        errors = (variable.tail(num_interior_nodes) - true_values).cwiseAbs();
        const double max_error = errors.maxCoeff();
        const double mean_error = errors.mean();
        const double max_value = true_values.cwiseAbs().maxCoeff();

        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
            "{:.2e}",
            time, max_error, mean_error, max_value);

        // Add current solution to visualization
        visualized_solution.segment(1, num_interior_nodes) =
            variable.tail(num_interior_nodes);
        scatter = figure.add_scatter();
        scatter.x(visualized_nodes);
        scatter.y(visualized_solution);
        scatter.mode("lines");
        scatter.name(fmt::format("t = {:.3f}", time));
    }

    // Save visualization to HTML file
    plotly_plotter::write_html("rbf_fd_wave_equation_1d_string.html", figure);
    NUM_COLLECT_LOG_INFO(logger, "Wrote rbf_fd_wave_equation_1d_string.html.");
}

auto main(int argc, const char** argv) -> int {
    // Load configuration file
    std::string_view config_file_path =
        "examples/pde/rbf_fd_wave_equation_1d_string.toml";
    if (argc == 2) {
        config_file_path = argv[1];  // Use command-line argument if provided
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    // Parse simulation parameters from TOML configuration file
    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "rbf_fd_wave_equation_1d_string.num_interior_nodes");
    const auto polynomial_order =
        parser.get<int>("rbf_fd_wave_equation_1d_string.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "rbf_fd_wave_equation_1d_string.num_neighbors");
    const auto wave_speed =
        parser.get<double>("rbf_fd_wave_equation_1d_string.wave_speed");
    const auto time_step_size =
        parser.get<double>("rbf_fd_wave_equation_1d_string.time_step_size");
    const auto final_time =
        parser.get<double>("rbf_fd_wave_equation_1d_string.final_time");

    // Log configuration parameters
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(logger, "Wave speed: {}", wave_speed);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(logger, "Final time: {}", final_time);

    // Generate spatial discretization nodes
    const auto nodes = generate_nodes(num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    // Assemble the ODE system matrix
    const auto problem = assemble_system(
        nodes, num_interior_nodes, polynomial_order, num_neighbors, wave_speed);

    // Solve the ODE system and visualize
    solve_system(problem, wave_speed, time_step_size, final_time, nodes,
        num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
