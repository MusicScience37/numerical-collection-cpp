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

// Example: Solving 2D Burgers' equation using RBF-FD method.
//
// This example solves the following 2D Burgers' equation:
//   ∂u/∂t + u ∂u/∂x = D(∂²u/∂x² + ∂²u/∂y²)  (0 < x < 1, 0 < y < 1, t > 0)
//
// Boundary conditions:
//   u = 0 on all boundaries (Dirichlet)
//
// Initial condition:
//   A smooth, compactly supported bump centered at (0.4, 0.5) with radius
//   r0 = 0.2:
//     u(x, y, 0) = exp(1 - 1 / (1 - r² / r0²))  for r < r0
//     u(x, y, 0) = 0                            otherwise
//   where r is the distance from the center.
//
// Solution method:
//   - Spatial discretization: RBF-FD (Radial Basis Function - Finite
//     Difference) with quasi-random Halton nodes
//   - Stabilization: Hyperviscosity using polyharmonic operator of order 3
//   - Time integration: Rosenbrock method (RODASPR) for the first-order ODE
//     system
//
// Output:
//   - Maximum and minimum values are logged at each time step
//   - Time evolution is visualized and saved as
//     rbf_fd_burgers_equation_2d.pvd and
//     rbf_fd_burgers_equation_2d_XXXX.vtp files for ParaView

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
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/operators/partial_derivative_operator.h"
#include "num_collect/rbf/operators/polyharmonic_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/eigen_triplets_util.h"
#include "num_collect/util/generate_rectangle_boundary_nodes_without_corners.h"
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

// Directory for output ParaView visualization files.
static constexpr std::string_view output_directory =
    "rbf_fd_burgers_equation_2d";

// Initial condition: a smooth, compactly supported bump centered at
// (0.4, 0.5) with radius 0.2. Used to set the initial solution.
static auto test_function(const position_type& position) -> double {
    constexpr double center_x = 0.4;
    constexpr double center_y = 0.5;
    constexpr double radius = 0.2;
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

// ODE problem for the 2D Burgers' equation with Dirichlet boundary
// conditions.
//
// The spatially discretized Burgers' equation takes the form:
//   du/dt = -u∘(B*u) + A*u
// where B is the RBF-FD approximation of ∂/∂x (used to form the nonlinear
// advection term -u∘(B*u) = -u ∂u/∂x), A is the RBF-FD approximation of
// D(∂²/∂x² + ∂²/∂y²) plus hyperviscosity for stabilization, and ∘ denotes
// elementwise multiplication.
// ODE variable: u at interior nodes only (boundary values are always zero).
class burgers_equation_ode_problem {
public:
    // Type of vectors.
    using vector_type = solution_type;

    // Type of coefficients matrix.
    using matrix_type = sparse_matrix_type;

    // Type of scalars.
    using scalar_type = typename vector_type::value_type;

    // Type of variables. (Used by ODE solvers.)
    using variable_type = vector_type;

    // Jacobian is not used for this problem.

    // Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true};

    burgers_equation_ode_problem(
        const matrix_type& diff_x_coeff, const matrix_type& linear_coeff)
        : diff_x_coeff_(diff_x_coeff), linear_coeff_(linear_coeff) {}

    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        partial_derivative_x_ = diff_x_coeff_ * variable;
        diff_coeff_ = -variable.cwiseProduct(partial_derivative_x_);
        diff_coeff_.noalias() += linear_coeff_ * variable;
    }

    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

private:
    // Coefficient matrix (B) approximating ∂/∂x, used to form the nonlinear
    // advection term -u ∂u/∂x.
    matrix_type diff_x_coeff_;

    // Coefficient matrix (A) for the diffusion and hyperviscosity terms.
    matrix_type linear_coeff_;

    // Differential coefficient.
    variable_type diff_coeff_;

    // Buffer for the partial derivative ∂u/∂x.
    variable_type partial_derivative_x_;
};

// Type alias for similarity with other examples and experiments.
using ode_problem_type = burgers_equation_ode_problem;

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
        num_collect::util::generate_rectangle_boundary_nodes_without_corners(
            position_type(0.0, 0.0), position_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    // Combine interior and boundary nodes.
    num_collect::util::vector<position_type> nodes;
    nodes.reserve(interior_nodes.size() + boundary_nodes.size());
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    return nodes;
}

// Assemble the coefficient matrices for the Burgers' equation ODE.
//
// Constructs two RBF-FD operator matrices used to form:
//   du/dt = -u∘(B*u) + A*u
// where B approximates ∂/∂x (used to form the nonlinear advection term
// -u ∂u/∂x) and A approximates
//   D(∂²/∂x² + ∂²/∂y²) + (-1)^(p-1) ε h^(2p) Δ^p  (p = 3),
// i.e. the diffusion term plus hyperviscosity for stabilization.
//
// Parameters:
//   nodes: All nodes (interior + boundary), where first num_interior_nodes
//     are interior nodes
//   num_interior_nodes: Number of interior nodes
//   polynomial_order: Order of polynomials in RBF-FD augmentation
//   num_neighbors: Number of neighboring nodes used in each RBF-FD stencil
//   diffusion_coefficient: Diffusion coefficient D in the equation
//   hyperviscosity_rate: Scaling factor ε for the hyperviscosity term
//
// Returns:
//   Nonlinear first-order ODE problem du/dt = -u∘(B*u) + A*u, where u is
//   the solution at interior nodes only
static auto assemble_system(
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double diffusion_coefficient,
    double hyperviscosity_rate) -> ode_problem_type {
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

    // Compute RBF-FD operators for interior nodes.
    const auto interior_nodes = nodes.first(num_interior_nodes);
    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    // ∂/∂x operator (B), used to form the nonlinear advection term.
    const auto diff_x_triplets = assembler.compute_rows(
        [](const position_type& position) {
            return num_collect::rbf::operators::partial_derivative_operator<
                position_type, 0>(position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher);
    // Diffusion and hyperviscosity operator (A).
    const auto linear_part_triplets = assembler.compute_rows(
        [diffusion_coefficient, hyperviscosity_coeff](
            const position_type& position) {
            return
                // Hyperviscosity term for stabilization.
                hyperviscosity_coeff *
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
    const auto diff_x_coeff = diff_x_triplets |
        filter_columns(0, num_interior_nodes) |
        to_sparse_matrix<sparse_matrix_type>(
            num_interior_nodes, num_interior_nodes);
    const auto linear_coeff = linear_part_triplets |
        filter_columns(0, num_interior_nodes) |
        to_sparse_matrix<sparse_matrix_type>(
            num_interior_nodes, num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(diff_x_coeff, linear_coeff);
}

// Solve the Burgers' equation ODE system and visualize results.
//
// Integrates the first-order ODE system in time using the RODASPR method
// (a Rosenbrock-type solver). At each time step, the current solution is
// written to a ParaView file. There is no analytical solution for this
// initial condition, so only the maximum and minimum values are logged.
//
// Parameters:
//   problem: ODE problem assembled by assemble_system
//   time_step_size: Time step for output (solver uses adaptive stepping
//     internally)
//   final_time: End time of simulation
//   nodes: All nodes (same as in assemble_system)
//   num_interior_nodes: Number of interior nodes
//
// Output:
//   - Logs the maximum and minimum values at each time step
//   - Saves visualization to rbf_fd_burgers_equation_2d/
//     directory in ParaView format (.pvd and .vtp files)
static void solve_system(const ode_problem_type& problem, double time_step_size,
    double final_time,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;

    std::filesystem::create_directories(output_directory);

    const std::string para_view_data_file_path =
        fmt::format("{}/rbf_fd_burgers_equation_2d.pvd", output_directory);
    para_view_time_steps_data_file_writer para_view_writer(
        para_view_data_file_path);

    // Initialize time and solution vector.
    num_collect::index_type time_index = 0;
    double time = 0.0;

    solution_type whole_variable = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        whole_variable(i) = test_function(nodes[i]);
    }
    std::string vtp_file_name =
        fmt::format("rbf_fd_burgers_equation_2d_{:04d}.vtp", time_index);
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
        vtp_file_name =
            fmt::format("rbf_fd_burgers_equation_2d_{:04d}.vtp", time_index);
        write_vtp_file_with_solution(
            fmt::format("{}/{}", output_directory, vtp_file_name), nodes,
            whole_variable);
        para_view_writer.add_time_step(time, vtp_file_name);
    }
}

auto main(int argc, const char** argv) -> int {
    // Load configuration file.
    std::string_view config_file_path =
        "examples/pde/rbf_fd_burgers_equation_2d.toml";
    if (argc == 2) {
        config_file_path = argv[1];  // Use command-line argument if provided.
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    // Parse simulation parameters from TOML configuration file.
    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "rbf_fd_burgers_equation_2d.num_interior_nodes");
    const auto num_boundary_nodes_per_edge =
        parser.get<num_collect::index_type>(
            "rbf_fd_burgers_equation_2d.num_boundary_nodes_per_edge");
    const auto polynomial_order =
        parser.get<int>("rbf_fd_burgers_equation_2d.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "rbf_fd_burgers_equation_2d.num_neighbors");
    const auto diffusion_coefficient =
        parser.get<double>("rbf_fd_burgers_equation_2d.diffusion_coefficient");
    const auto hyperviscosity_rate =
        parser.get<double>("rbf_fd_burgers_equation_2d.hyperviscosity_rate");
    const auto time_step_size =
        parser.get<double>("rbf_fd_burgers_equation_2d.time_step_size");
    const auto final_time =
        parser.get<double>("rbf_fd_burgers_equation_2d.final_time");

    // Log configuration parameters.
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Number of boundary nodes per edge: {}",
        num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
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
    const auto problem =
        assemble_system(nodes, num_interior_nodes, polynomial_order,
            num_neighbors, diffusion_coefficient, hyperviscosity_rate);

    // Solve the ODE system and visualize.
    solve_system(
        problem, time_step_size, final_time, nodes, num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
