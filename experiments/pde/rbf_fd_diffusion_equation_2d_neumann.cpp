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
 * \brief Experiments to solve 2D diffusion equation with Neumann boundary
 * conditions using RBF-FD method.
 *
 * - Region is [0, 1] x [0, 0.5].
 * - \f$x = 0\f$, \f$x = 1\f$, and \f$y = 0\f$ are Dirichlet boundaries,
 *   and \f$y = 0.5\f$ is Neumann boundary.
 */
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
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
#include "toml_parser.h"
#include "write_vtp_file_for_comparison.h"

using position_type = Eigen::Vector2d;
using solution_type = Eigen::VectorXd;
using sparse_matrix_type = Eigen::SparseMatrix<double,
    Eigen::RowMajor>;  // BiCGstab works better with row-major format.

static constexpr std::string_view output_directory =
    "rbf_fd_diffusion_equation_2d_neumann";

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

/*!
 * \brief Solve the assembled system.
 *
 * \param[in] problem ODE problem to solve.
 * \param[in] diffusion_coefficient Diffusion coefficient.
 * \param[in] time_step_size Time step size.
 * \param[in] final_time Final time.
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 */
static void solve_system(const ode_problem_type& problem,
    double diffusion_coefficient, double time_step_size, double final_time,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;

    std::filesystem::create_directories(output_directory);

    num_collect::index_type time_index = 0;
    double time = 0.0;
    solution_type solution = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        solution(i) = test_function(nodes[i], time, diffusion_coefficient);
    }
    solution_type true_values = solution;
    solution_type errors = solution_type::Zero(nodes.size());
    std::string file_path =
        fmt::format("{}/rbf_fd_diffusion_equation_2d_neumann_{:04d}.vtp",
            output_directory, time_index);
    write_vtp_file_for_comparison(
        file_path, nodes, solution, true_values, errors);

    num_collect::util::vector<double> time_list;
    time_list.push_back(time);

    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_solver<ode_problem_type>;
    solver_type solver(problem);
    solver.init(time, solution);
    while (time < final_time) {
        double next_time = time + time_step_size;
        constexpr double time_threshold = 1e-4;
        if (next_time > final_time - time_threshold) {
            next_time = final_time;
        }
        solver.solve_till(next_time);

        time = solver.time();
        time_list.push_back(time);
        ++time_index;

        solution = solver.variable();
        for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
            true_values(i) =
                test_function(nodes[i], time, diffusion_coefficient);
        }
        errors = (solution - true_values).cwiseAbs();

        const double max_error = errors.maxCoeff();
        const double mean_error = errors.head(num_interior_nodes).mean();
        const double max_value = true_values.cwiseAbs().maxCoeff();
        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
            "{:.2e}",
            time, max_error, mean_error, max_value);

        file_path =
            fmt::format("{}/rbf_fd_diffusion_equation_2d_neumann_{:04d}.vtp",
                output_directory, time_index);
        write_vtp_file_for_comparison(
            file_path, nodes, solution, true_values, errors);
    }

    const std::string para_view_data_file_path = fmt::format(
        "{}/rbf_fd_diffusion_equation_2d_neumann.pvd", output_directory);
    std::ofstream para_view_data_file(para_view_data_file_path);
    fmt::print(para_view_data_file, R"(<?xml version="1.0"?>
<VTKFile type="Collection" version="0.1" byte_order="LittleEndian">
  <Collection>
)");
    for (num_collect::index_type i = 0; i < time_list.size(); ++i) {
        fmt::print(para_view_data_file,
            R"(    <DataSet timestep="{:.2e}" file="rbf_fd_diffusion_equation_2d_neumann_{:04d}.vtp"/>
)",
            time_list[i], i);
    }
    fmt::print(para_view_data_file,
        R"(  </Collection>
</VTKFile>)");
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_diffusion_equation_2d_neumann.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "rbf_fd_diffusion_equation_2d_neumann.num_interior_nodes");
    const auto num_boundary_nodes_per_edge =
        parser.get<num_collect::index_type>(
            "rbf_fd_diffusion_equation_2d_neumann.num_boundary_nodes_per_edge");
    const auto polynomial_order = parser.get<int>(
        "rbf_fd_diffusion_equation_2d_neumann.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "rbf_fd_diffusion_equation_2d_neumann.num_neighbors");
    const auto diffusion_coefficient = parser.get<double>(
        "rbf_fd_diffusion_equation_2d_neumann.diffusion_coefficient");
    const auto time_step_size = parser.get<double>(
        "rbf_fd_diffusion_equation_2d_neumann.time_step_size");
    const auto final_time =
        parser.get<double>("rbf_fd_diffusion_equation_2d_neumann.final_time");
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

    const auto nodes =
        generate_nodes(num_interior_nodes, num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    const auto problem =
        assemble_system(nodes, num_interior_nodes, num_boundary_nodes_per_edge,
            polynomial_order, num_neighbors, diffusion_coefficient);
    solve_system(problem, diffusion_coefficient, time_step_size, final_time,
        nodes, num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
