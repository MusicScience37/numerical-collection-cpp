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
 * \brief Experiments to solve the Poisson equation in 1D with RBF-FD method.
 */
#include <algorithm>
#include <cmath>
#include <string>
#include <tuple>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>
#include <toml++/toml.h>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_assembler.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

using variable_type = double;
using sparse_matrix_type = Eigen::SparseMatrix<double,
    Eigen::RowMajor>;  // BiCGstab works better with row-major format.

static auto test_function(const variable_type& variable) -> double {
    return std::sin(variable * num_collect::base::pi<double>);
}

static auto test_function_laplacian(const variable_type& variable) -> double {
    constexpr double coeff =
        -num_collect::base::pi<double> * num_collect::base::pi<double>;
    return coeff * std::sin(variable * num_collect::base::pi<double>);
}

/*!
 * \brief Generate nodes.
 *
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \return Generated nodes. First num_interior_nodes nodes are in the interior,
 * and the rest are on the boundary in the counter-clockwise order.
 */
static auto generate_nodes(num_collect::index_type num_interior_nodes)
    -> num_collect::util::vector<variable_type> {
    auto interior_nodes =
        num_collect::rbf::generate_1d_halton_nodes<double>(num_interior_nodes);
    std::ranges::sort(interior_nodes);
    num_collect::util::vector<variable_type> nodes;
    nodes.reserve(interior_nodes.size() + 2);
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.push_back(0.0);
    nodes.push_back(1.0);
    return nodes;
}

/*!
 * \brief Assemble the system of the Poisson equation.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] polynomial_order Order of polynomials used in RBF-FD. -1 for no
 * polynomials.
 * \param[in] num_neighbors Number of neighbors used in RBF-FD.
 * \param[in] length_parameter_scale Scale of length parameters in RBF-FD.
 * \return Coefficient matrix and right-hand side vector.
 */
static auto assemble_system(
    num_collect::util::vector_view<const variable_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double length_parameter_scale)
    -> std::tuple<sparse_matrix_type, Eigen::VectorXd> {
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd right_vec(nodes.size());

    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);

    // Interior nodes.
    const auto interior_nodes = nodes.first(num_interior_nodes);
    if (polynomial_order < 0) {
        // RBF-FD without polynomials.
        // Without polynomials, no RBF I tested worked.
        // Gaussian RBF tends to give large errors with many points.
        // Thin plate spline RBF won't work without polynomials.
        // TODO Research why Gaussian RBF is not working.
        using assembler_type =
            num_collect::rbf::rbf_fd_assembler<variable_type>;
        assembler_type assembler;
        assembler.num_neighbors(num_neighbors);
        assembler.length_parameter_scale(length_parameter_scale);
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows<operator_type>(interior_nodes, nodes,
            column_variables_nearest_neighbor_searcher, triplets, row_offset,
            column_offset);
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            right_vec(i) = test_function_laplacian(interior_nodes[i]);
        }
    } else {
        // RBF-FD with polynomials.
        // Gaussian RBF did not work well, but thin plate spline RBF worked
        // well.
        // TODO Research why Gaussian RBF is not working.
        using assembler_type =
            num_collect::rbf::phs_rbf_fd_polynomial_assembler<variable_type>;
        assembler_type assembler(polynomial_order);
        assembler.num_neighbors(num_neighbors);
        assembler.length_parameter_scale(length_parameter_scale);
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows<operator_type>(interior_nodes, nodes,
            column_variables_nearest_neighbor_searcher, triplets, row_offset,
            column_offset);
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            right_vec(i) = test_function_laplacian(interior_nodes[i]);
        }
    }

    // Boundary nodes.
    for (num_collect::index_type i = num_interior_nodes; i < nodes.size();
        ++i) {
        const auto index_in_triplet = static_cast<int>(i);
        triplets.emplace_back(index_in_triplet, index_in_triplet, 1.0);
        right_vec(i) = test_function(nodes[i]);
    }

    sparse_matrix_type mat(nodes.size(), nodes.size());
    mat.setFromTriplets(triplets.begin(), triplets.end());

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return {mat, right_vec};
}

/*!
 * \brief Solve the system of the Poisson equation.
 *
 * \param[in] mat System matrix.
 * \param[in] right_vec Right-hand side vector.
 * \return Solution vector.
 */
static auto solve_system(const sparse_matrix_type& mat,
    const Eigen::VectorXd& right_vec) -> Eigen::VectorXd {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start to solve the system.");

    Eigen::BiCGSTAB<sparse_matrix_type> solver;
    solver.compute(mat);
    Eigen::VectorXd solution = solver.solve(right_vec);

    NUM_COLLECT_LOG_INFO(logger, "Finished to solve the system.");

    return solution;
}

/*!
 * \brief Evaluate the solution.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] solution Solution vector.
 * \return True values and errors.
 */
static auto evaluate_solution(
    const num_collect::util::vector_view<const variable_type>& nodes,
    const Eigen::VectorXd& solution)
    -> std::tuple<Eigen::VectorXd, Eigen::VectorXd> {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start to evaluate the solution.");

    Eigen::VectorXd true_values(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        true_values(i) = test_function(nodes[i]);
    }
    const Eigen::VectorXd errors = (solution - true_values).cwiseAbs();
    const double max_error = errors.maxCoeff();
    const double mean_error = errors.mean();
    const double max_value = true_values.cwiseAbs().maxCoeff();
    NUM_COLLECT_LOG_INFO(logger,
        "Max error: {:.2e}, Mean error: {:.2e}, Max value: {:.2e}", max_error,
        mean_error, max_value);

    return {true_values, errors};
}

static void write_plots(
    const num_collect::util::vector_view<const variable_type>& nodes,
    const Eigen::VectorXd& solution, const Eigen::VectorXd& true_values,
    const Eigen::VectorXd& errors) {
    num_collect::logging::logger logger;

    plotly_plotter::figure figure;

    auto scatter = figure.add_scatter_gl();
    scatter.x(nodes);
    scatter.y(solution);
    scatter.mode("lines");
    scatter.name("Solution");

    scatter = figure.add_scatter_gl();
    scatter.x(nodes);
    scatter.y(true_values);
    scatter.mode("lines");
    scatter.name("True values");

    scatter = figure.add_scatter_gl();
    scatter.x(nodes);
    scatter.y(errors);
    scatter.mode("lines");
    scatter.name("Errors");

    figure.x_title("Position");
    figure.y_title("Value");
    figure.title(
        "The solution of the Poisson equation with RBF-FD method in 1D");

    const std::string file_name = "rbf_fd_poisson_equation_1d.html";
    plotly_plotter::write_html(file_name, figure);

    NUM_COLLECT_LOG_INFO(logger, "Wrote {}.", file_name);
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_poisson_equation_1d.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    toml::table config = toml::parse_file(config_file_path);
    const auto num_interior_nodes =
        config.at_path("rbf_fd_poisson_equation_1d.num_interior_nodes")
            .value<num_collect::index_type>()
            .value();
    const auto polynomial_order =
        config.at_path("rbf_fd_poisson_equation_1d.polynomial_order")
            .value<int>()
            .value();
    const auto num_neighbors =
        config.at_path("rbf_fd_poisson_equation_1d.num_neighbors")
            .value<num_collect::index_type>()
            .value();
    const auto length_parameter_scale =
        config.at_path("rbf_fd_poisson_equation_1d.length_parameter_scale")
            .value<double>()
            .value();
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(
        logger, "Scale of length parameters: {}", length_parameter_scale);

    const auto nodes = generate_nodes(num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    const auto [mat, right_vec] = assemble_system(nodes, num_interior_nodes,
        polynomial_order, num_neighbors, length_parameter_scale);
    const auto solution = solve_system(mat, right_vec);

    const auto [true_values, errors] = evaluate_solution(nodes, solution);
    write_plots(nodes, solution, true_values, errors);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
