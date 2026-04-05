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
 * \brief Experiments to solve 2D diffusion equation with Dirichlet boundary
 * conditions using RBF-FD method.
 */
#include <cmath>
#include <filesystem>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <toml++/toml.h>

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
#include "write_vtp_file_for_comparison.h"

using position_type = Eigen::Vector2d;
using solution_type = Eigen::VectorXd;
using sparse_matrix_type = Eigen::SparseMatrix<double,
    Eigen::RowMajor>;  // BiCGstab works better with row-major format.

static constexpr std::string_view output_directory =
    "rbf_fd_diffusion_equation_2d_dirichlet";

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
    const auto interior_nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_interior_nodes);
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    num_collect::util::vector<position_type> nodes;
    nodes.reserve(interior_nodes.size() + boundary_nodes.size());
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    return nodes;
}

using ode_problem_type =
    num_collect::ode::problems::linear_first_order_ode_problem<solution_type,
        sparse_matrix_type>;

/*!
 * \brief Assemble the system of the diffusion equation.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] polynomial_order Order of polynomials used in RBF-FD. -1 for no
 * polynomials.
 * \param[in] num_neighbors Number of neighbors used in RBF-FD.
 * \param[in] diffusion_coefficient Diffusion coefficient.
 * \return ODE problem to solve.
 */
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

    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

    const auto interior_nodes = nodes.first(num_interior_nodes);
    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    constexpr num_collect::index_type row_offset = 0;
    constexpr num_collect::index_type column_offset = 0;
    assembler.compute_rows<operator_type>(interior_nodes, nodes,
        column_variables_nearest_neighbor_searcher, triplets, row_offset,
        column_offset);

    sparse_matrix_type whole_coefficients(num_interior_nodes, nodes.size());
    whole_coefficients.setFromTriplets(triplets.begin(), triplets.end());
    whole_coefficients *= diffusion_coefficient;

    const sparse_matrix_type variable_coefficients =
        whole_coefficients.leftCols(num_interior_nodes);
    const solution_type constant_term = solution_type::Zero(num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(variable_coefficients, constant_term);
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
    solution_type whole_variable = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        whole_variable(i) =
            test_function(nodes[i], time, diffusion_coefficient);
    }
    solution_type true_values = whole_variable;
    solution_type errors = solution_type::Zero(nodes.size());
    std::string file_path =
        fmt::format("{}/rbf_fd_diffusion_equation_2d_dirichlet_{:04d}.vtp",
            output_directory, time_index);
    write_vtp_file_for_comparison(
        file_path, nodes, whole_variable, true_values, errors);

    num_collect::util::vector<double> time_list;
    time_list.push_back(time);

    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_solver<ode_problem_type>;
    solver_type solver(problem);
    const solution_type initial_variable =
        whole_variable.head(num_interior_nodes);
    solver.init(time, initial_variable);
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

        whole_variable.head(num_interior_nodes) = solver.variable();
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            true_values(i) =
                test_function(nodes[i], time, diffusion_coefficient);
        }
        errors = (whole_variable - true_values).cwiseAbs();

        const double max_error = errors.maxCoeff();
        const double mean_error = errors.head(num_interior_nodes).mean();
        const double max_value = true_values.cwiseAbs().maxCoeff();
        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
            "{:.2e}",
            time, max_error, mean_error, max_value);

        file_path =
            fmt::format("{}/rbf_fd_diffusion_equation_2d_dirichlet_{:04d}.vtp",
                output_directory, time_index);
        write_vtp_file_for_comparison(
            file_path, nodes, whole_variable, true_values, errors);
    }

    const std::string para_view_data_file_path = fmt::format(
        "{}/rbf_fd_diffusion_equation_2d_dirichlet.pvd", output_directory);
    std::ofstream para_view_data_file(para_view_data_file_path);
    fmt::print(para_view_data_file, R"(<?xml version="1.0"?>
<VTKFile type="Collection" version="0.1" byte_order="LittleEndian">
  <Collection>
)");
    for (num_collect::index_type i = 0; i < time_list.size(); ++i) {
        fmt::print(para_view_data_file,
            R"(    <DataSet timestep="{:.2e}" file="rbf_fd_diffusion_equation_2d_dirichlet_{:04d}.vtp"/>
)",
            time_list[i], i);
    }
    fmt::print(para_view_data_file,
        R"(  </Collection>
</VTKFile>)");
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_diffusion_equation_2d_dirichlet.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    toml::table config = toml::parse_file(config_file_path);
    const auto num_interior_nodes =
        config
            .at_path(
                "rbf_fd_diffusion_equation_2d_dirichlet.num_interior_nodes")
            .value<num_collect::index_type>()
            .value();
    const auto num_boundary_nodes_per_edge =
        config
            .at_path(
                "rbf_fd_diffusion_equation_2d_dirichlet.num_boundary_nodes_per_"
                "edge")
            .value<num_collect::index_type>()
            .value();
    const auto polynomial_order =
        config
            .at_path("rbf_fd_diffusion_equation_2d_dirichlet.polynomial_order")
            .value<int>()
            .value();
    const auto num_neighbors =
        config.at_path("rbf_fd_diffusion_equation_2d_dirichlet.num_neighbors")
            .value<num_collect::index_type>()
            .value();
    const auto diffusion_coefficient =
        config
            .at_path(
                "rbf_fd_diffusion_equation_2d_dirichlet.diffusion_coefficient")
            .value<double>()
            .value();
    const auto time_step_size =
        config.at_path("rbf_fd_diffusion_equation_2d_dirichlet.time_step_size")
            .value<double>()
            .value();
    const auto final_time =
        config.at_path("rbf_fd_diffusion_equation_2d_dirichlet.final_time")
            .value<double>()
            .value();
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

    const auto problem = assemble_system(nodes, num_interior_nodes,
        polynomial_order, num_neighbors, diffusion_coefficient);
    solve_system(problem, diffusion_coefficient, time_step_size, final_time,
        nodes, num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
