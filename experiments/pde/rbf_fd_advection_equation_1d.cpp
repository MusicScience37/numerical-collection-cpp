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

// Experiment: Solving 1D advection equation using RBF-FD method.

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

#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/gradient_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"
#include "toml_parser.h"

// Type definitions for better readability.
// Node positions (x-coordinates).
using position_type = double;
// Solution vectors.
using solution_type = Eigen::VectorXd;
// Sparse matrix (row-major for performance).
using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

constexpr double left_boundary_position = 0.0;

// Analytical solution
static auto test_function(const position_type& position, double time,
    double advection_velocity) -> double {
    constexpr double sigma = 0.1;
    constexpr double first_center = 0.3;

    double relative_position =
        position - advection_velocity * time - first_center;
    constexpr double coeff = 1.0 / (2.0 * sigma * sigma);
    return std::exp(-coeff * relative_position * relative_position);
}

class advection_ode_problem {
public:
    //! Type of vectors.
    using vector_type = solution_type;

    //! Type of coefficients matrix.
    using matrix_type = sparse_matrix_type;

    //! Type of scalars.
    using scalar_type = typename vector_type::value_type;

    //! Type of variables. (Used by ODE solvers.)
    using variable_type = vector_type;

    //! Type of Jacobian. (Used by ODE solvers.)
    using jacobian_type = matrix_type;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true, .jacobian = true};

    /*!
     * \brief Constructor.
     *
     * \param[in] stiffness_matrix Stiffness matrix.
     * \param[in] left_boundary_coeff Coefficients for the left boundary
     * condition.
     * \param[in] advection_velocity Velocity of advection.
     */
    advection_ode_problem(const matrix_type& stiffness_matrix,
        const vector_type& left_boundary_coeff, double advection_velocity)
        : stiffness_matrix_(stiffness_matrix),
          left_boundary_coeff_(left_boundary_coeff),
          advection_velocity_(advection_velocity) {}
    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] time Time.
     * \param[in] variable Variable.
     */
    void evaluate_on(scalar_type time, const variable_type& variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        diff_coeff_ = stiffness_matrix_ * variable;
        const double left_boundary_value =
            test_function(left_boundary_position, time, advection_velocity_);
        diff_coeff_ += left_boundary_coeff_ * left_boundary_value;
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

    /*!
     * \brief Get the Jacobian.
     *
     * \return Jacobian.
     */
    [[nodiscard]] auto jacobian() const noexcept -> const jacobian_type& {
        return stiffness_matrix_;
    }

private:
    //! Stiffness matrix.
    matrix_type stiffness_matrix_;

    //! Coefficients for the left boundary condition.
    vector_type left_boundary_coeff_;

    //! Speed of advection.
    double advection_velocity_;

    //! Differential coefficient.
    variable_type diff_coeff_;
};

using ode_problem_type = advection_ode_problem;

static auto generate_nodes(num_collect::index_type num_interior_nodes)
    -> num_collect::util::vector<position_type> {
    // Generate interior nodes using Halton sequence.
    auto interior_nodes =
        num_collect::rbf::generate_1d_halton_nodes<double>(num_interior_nodes);
    std::ranges::sort(interior_nodes);

    // Combine interior and boundary nodes.
    num_collect::util::vector<position_type> nodes;
    nodes.reserve(interior_nodes.size() + 2);
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.push_back(1.0);  // Right boundary
    nodes.push_back(0.0);  // Left boundary
    return nodes;
}

static auto assemble_system(
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double advection_velocity)
    -> ode_problem_type {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    // Set up RBF-FD assembler with PHS (Polyharmonic Spline) + polynomial
    // augmentation.
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

    num_collect::util::vector<Eigen::Triplet<double>> advection_triplets;
    const auto interior_and_right_boundary_nodes =
        nodes.first(num_interior_nodes + 1);
    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    constexpr num_collect::index_type row_offset = 0;
    constexpr num_collect::index_type column_offset = 0;
    assembler.compute_rows(
        [advection_velocity](const position_type& position) {
            // TODO Stabilization.
            return -advection_velocity *
                num_collect::rbf::operators::gradient_operator<position_type>(
                    position);
        },
        interior_and_right_boundary_nodes, nodes,
        column_variables_nearest_neighbor_searcher, advection_triplets,
        row_offset, column_offset);

    sparse_matrix_type advection_coefficients(
        num_interior_nodes + 1, nodes.size());
    advection_coefficients.setFromTriplets(
        advection_triplets.begin(), advection_triplets.end());

    const sparse_matrix_type stiffness_matrix =
        advection_coefficients.leftCols(num_interior_nodes + 1);
    const solution_type left_boundary_coeff =
        advection_coefficients.col(num_interior_nodes + 1);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(
        stiffness_matrix, left_boundary_coeff, advection_velocity);
}

static void solve_system(const ode_problem_type& problem,
    double advection_velocity, double time_step_size, double final_time,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;

    NUM_COLLECT_LOG_INFO(logger, "Start to solve the system.");

    // Initialize time and solution vector.
    double time = 0.0;
    solution_type variable = solution_type::Zero(num_interior_nodes + 1);

    // Set initial condition
    for (num_collect::index_type i = 0; i < num_interior_nodes + 1; ++i) {
        variable(i) = test_function(nodes[i], time, advection_velocity);
    }

    // Variables for error evaluation.
    solution_type true_values = variable;
    solution_type errors = solution_type::Zero(num_interior_nodes + 1);

    // Prepare nodes for visualization (including boundary points).
    num_collect::util::vector<position_type> visualized_nodes;
    visualized_nodes.reserve(num_interior_nodes + 2);
    visualized_nodes.push_back(0.0);  // Left boundary
    visualized_nodes.insert(visualized_nodes.end(), nodes.begin(),
        nodes.begin() + num_interior_nodes);  // Interior nodes
    visualized_nodes.push_back(1.0);          // Right boundary

    // Initialize visualization.
    solution_type visualized_solution =
        solution_type::Zero(visualized_nodes.size());
    visualized_solution(0) =
        test_function(left_boundary_position, time, advection_velocity);
    visualized_solution.segment(1, num_interior_nodes + 1) = variable;

    plotly_plotter::figure figure;
    auto scatter = figure.add_scatter();
    scatter.x(visualized_nodes);
    scatter.y(visualized_solution);
    scatter.mode("lines");
    scatter.name(fmt::format("t = {:.3f}", time));

    // Set up ODE solver.
    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_adaptive_step_solver<
            ode_problem_type>;
    solver_type solver(problem);
    solver.init(time, variable);

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
        variable = solver.variable();

        // Compute analytical solution at current time.
        for (num_collect::index_type i = 0; i < num_interior_nodes + 1; ++i) {
            true_values(i) = test_function(nodes[i], time, advection_velocity);
        }

        // Calculate errors.
        errors = (variable - true_values).cwiseAbs();
        const double max_error = errors.maxCoeff();
        const double mean_error = errors.mean();
        const double max_value = true_values.cwiseAbs().maxCoeff();
        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
            "{:.2e}",
            time, max_error, mean_error, max_value);

        // Add current solution to visualization.
        visualized_solution(0) =
            test_function(left_boundary_position, time, advection_velocity);
        visualized_solution.segment(1, num_interior_nodes + 1) = variable;
        scatter = figure.add_scatter();
        scatter.x(visualized_nodes);
        scatter.y(visualized_solution);
        scatter.mode("lines");
        scatter.name(fmt::format("t = {:.3f}", time));
    }

    // Save visualization to HTML file.
    plotly_plotter::write_html("rbf_fd_advection_equation_1d.html", figure);
    NUM_COLLECT_LOG_INFO(logger, "Wrote rbf_fd_advection_equation_1d.html.");
}

auto main(int argc, const char** argv) -> int {
    // Load configuration file.
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_advection_equation_1d.toml";
    if (argc == 2) {
        config_file_path = argv[1];  // Use command-line argument if provided.
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    // Parse simulation parameters from TOML configuration file.
    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "rbf_fd_advection_equation_1d.num_interior_nodes");
    const auto polynomial_order =
        parser.get<int>("rbf_fd_advection_equation_1d.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "rbf_fd_advection_equation_1d.num_neighbors");
    const auto advection_velocity =
        parser.get<double>("rbf_fd_advection_equation_1d.advection_velocity");
    const auto time_step_size =
        parser.get<double>("rbf_fd_advection_equation_1d.time_step_size");
    const auto final_time =
        parser.get<double>("rbf_fd_advection_equation_1d.final_time");

    // Log configuration parameters.
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(logger, "Advection velocity: {}", advection_velocity);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(logger, "Final time: {}", final_time);

    // Generate spatial discretization nodes.
    const auto nodes = generate_nodes(num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    // Assemble the ODE system matrix.
    const auto problem = assemble_system(nodes, num_interior_nodes,
        polynomial_order, num_neighbors, advection_velocity);

    // Solve the ODE system and visualize.
    solve_system(problem, advection_velocity, time_step_size, final_time, nodes,
        num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
