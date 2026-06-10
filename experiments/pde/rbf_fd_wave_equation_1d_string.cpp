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
 * \brief Experiments to solve 1D wave equation on a string using RBF-FD method.
 */
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

using position_type = double;
using solution_type = Eigen::VectorXd;
using sparse_matrix_type = Eigen::SparseMatrix<double,
    Eigen::RowMajor>;  // BiCGstab works better with row-major format.

static auto test_function(
    const position_type& position, double time, double wave_speed) -> double {
    return std::sin(num_collect::pi<double> * position) *
        std::cos(wave_speed * num_collect::pi<double> * time);
}

/*!
 * \brief Generate nodes.
 *
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \return Generated nodes. First num_interior_nodes nodes are in the interior,
 * and the rest are on the boundary.
 */
static auto generate_nodes(num_collect::index_type num_interior_nodes)
    -> num_collect::util::vector<position_type> {
    auto interior_nodes =
        num_collect::rbf::generate_1d_halton_nodes<double>(num_interior_nodes);
    std::ranges::sort(interior_nodes);
    num_collect::util::vector<position_type> nodes;
    nodes.reserve(interior_nodes.size() + 2);
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.push_back(0.0);
    nodes.push_back(1.0);
    return nodes;
}

using ode_problem_type =
    num_collect::ode::problems::linear_first_order_ode_problem<solution_type,
        sparse_matrix_type>;
// Variables of ODE problem are [du/dt^\top, u^\top]^\top.

/*!
 * \brief Assemble the system of the wave equation.
 *
 * \param[in] nodes Nodes generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] polynomial_order Order of polynomials used in RBF-FD.
 * \param[in] num_neighbors Number of neighbors used in RBF-FD.
 * \param[in] wave_speed Wave speed.
 * \return ODE problem to solve.
 */
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

    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

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

    // Boundary conditions are u = 0 at both ends, so constant term is zero.
    const solution_type constant_term =
        solution_type::Zero(2 * num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(coefficients, constant_term);
}

/*!
 * \brief Solve the assembled system.
 *
 * \param[in] problem ODE problem to solve.
 * \param[in] wave_speed Wave speed.
 * \param[in] time_step_size Time step size.
 * \param[in] final_time Final time.
 * \param[in] nodes Nodes generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 */
static void solve_system(const ode_problem_type& problem, double wave_speed,
    double time_step_size, double final_time,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start to solve the system.");

    double time = 0.0;
    solution_type variable = solution_type::Zero(2 * num_interior_nodes);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        variable(i + num_interior_nodes) =
            test_function(nodes[i], time, wave_speed);
    }
    solution_type true_values = variable.tail(num_interior_nodes);
    solution_type errors = solution_type::Zero(num_interior_nodes);

    num_collect::util::vector<position_type> visualized_nodes;
    visualized_nodes.reserve(num_interior_nodes + 2);
    visualized_nodes.push_back(0.0);
    visualized_nodes.insert(visualized_nodes.end(), nodes.begin(),
        nodes.begin() + num_interior_nodes);
    visualized_nodes.push_back(1.0);
    solution_type visualized_solution =
        solution_type::Zero(visualized_nodes.size());
    visualized_solution.segment(1, num_interior_nodes) =
        variable.tail(num_interior_nodes);
    plotly_plotter::figure figure;
    auto scatter = figure.add_scatter();
    scatter.x(visualized_nodes);
    scatter.y(visualized_solution);
    scatter.mode("lines");
    scatter.name(fmt::format("t = {:.3f}", time));

    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_adaptive_step_solver<
            ode_problem_type>;
    solver_type solver(problem);
    solver.init(time, variable);
    while (time < final_time) {
        double next_time = time + time_step_size;
        constexpr double time_threshold = 1e-4;
        if (next_time > final_time - time_threshold) {
            next_time = final_time;
        }
        solver.solve_until(next_time);

        time = solver.time();

        variable = solver.variable();
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            true_values(i) = test_function(nodes[i], time, wave_speed);
        }
        errors = (variable.tail(num_interior_nodes) - true_values).cwiseAbs();

        const double max_error = errors.maxCoeff();
        const double mean_error = errors.mean();
        const double max_value = true_values.cwiseAbs().maxCoeff();
        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
            "{:.2e}",
            time, max_error, mean_error, max_value);

        visualized_solution.segment(1, num_interior_nodes) =
            variable.tail(num_interior_nodes);
        scatter = figure.add_scatter();
        scatter.x(visualized_nodes);
        scatter.y(visualized_solution);
        scatter.mode("lines");
        scatter.name(fmt::format("t = {:.3f}", time));
    }

    plotly_plotter::write_html("rbf_fd_wave_equation_1d_string.html", figure);
    NUM_COLLECT_LOG_INFO(logger, "Wrote rbf_fd_wave_equation_1d_string.html.");
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_wave_equation_1d_string.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

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
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(logger, "Wave speed: {}", wave_speed);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(logger, "Final time: {}", final_time);

    const auto nodes = generate_nodes(num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    const auto problem = assemble_system(
        nodes, num_interior_nodes, polynomial_order, num_neighbors, wave_speed);
    solve_system(problem, wave_speed, time_step_size, final_time, nodes,
        num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
