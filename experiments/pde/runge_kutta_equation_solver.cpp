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
 * \brief Experiment of solvers of equations in Runge-Kutta methods.
 *
 * This solves a time step of 2D diffusion equation with Dirichlet boundary
 * conditions using Crank-Nicolson method using several solvers for the
 * equations in the method.
 * In this experiment, the Jacobian matrices of the equations are not used
 * for the future implementation of non-linear problems.
 */
#include <chrono>
#include <cmath>
#include <concepts>
#include <limits>
#include <ratio>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "num_collect/base/constants.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/runge_kutta/inexact_newton_update_equation_solver.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"
#include "num_prob_collect/ode/no_jacobian_linear_first_order_ode_problem.h"
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
    num_prob_collect::ode::no_jacobian_linear_first_order_ode_problem<
        solution_type, sparse_matrix_type>;

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
    assembler.compute_rows(
        [diffusion_coefficient](const position_type& position) {
            return diffusion_coefficient * operator_type(position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher,
        triplets, row_offset, column_offset);

    sparse_matrix_type whole_coefficients(num_interior_nodes, nodes.size());
    whole_coefficients.setFromTriplets(triplets.begin(), triplets.end());

    const sparse_matrix_type variable_coefficients =
        whole_coefficients.leftCols(num_interior_nodes);
    const solution_type constant_term = solution_type::Zero(num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return ode_problem_type(variable_coefficients, constant_term);
}

/*!
 * \brief Solve for a time step with inexact Newton method.
 *
 * \param[in] problem Problem.
 * \param[in] time_step_size Time step size.
 * \param[in] time Current time.
 * \param[in,out] solution Solution vector.
 */
static void step_with_inexact_newton(ode_problem_type& problem,
    double time_step_size, double& time, solution_type& solution) {
    using formula_solver_type =
        num_collect::ode::runge_kutta::inexact_newton_update_equation_solver<
            ode_problem_type>;
    formula_solver_type formula_solver;

    constexpr double slope_coeff =
        0.5;  // Coefficient of slope in Crank-Nicolson.
    formula_solver.update_jacobian(
        problem, time, time_step_size, solution, slope_coeff);

    problem.evaluate_on(
        time, solution, num_collect::ode::evaluation_type{.diff_coeff = true});
    const solution_type k1 = problem.diff_coeff();
    solution_type z2 = time_step_size * slope_coeff * k1;
    formula_solver.init(time + time_step_size, z2, z2);
    formula_solver.solve();

    solution += z2;
    time += time_step_size;
}

/*!
 * \brief Perform fixed-point iteration.
 *
 * \tparam Function Type of the function to calculate the updated solution
 * without relaxation.
 * \param[in] function Function to calculate the updated solution without
 * relaxation.
 * \param[in,out] solution Solution vector.
 * \param[in] relaxation_coefficient Initial value of the relaxation coefficient
 * for the fixed-point iteration.
 * \param[in] tolerances Error tolerances.
 */
template <
    std::invocable<const solution_type& /*input*/, solution_type& /*output*/>
        Function>
static void fixed_point_iteration(Function&& function, solution_type& solution,
    double relaxation_coefficient,
    const num_collect::ode::error_tolerances<solution_type>& tolerances) {
    num_collect::index_type iterations = 0;
    double error_norm = 0.0;
    double previous_error_norm = 0.0;
    double error_reduction_rate = std::numeric_limits<double>::quiet_NaN();

    constexpr auto log_tag =
        num_collect::logging::log_tag_view("fixed_point_iteration");
    num_collect::logging::logger logger(log_tag);
    num_collect::logging::iterations::iteration_logger iteration_logger(logger);
    iteration_logger.append("Iter.", iterations);
    iteration_logger.append("Error", error_norm);
    iteration_logger.append("Err. Reduction", error_reduction_rate);
    iteration_logger.append("Relax. Coeff.", relaxation_coefficient);

    solution_type error;
    function(solution, error);
    error -= solution;
    error_norm = tolerances.calc_norm(solution, error);
    previous_error_norm = error_norm;
    solution_type non_relaxed_update = error;
    solution_type previous_solution;
    iteration_logger.write_iteration();
    while (iterations < 10000) {
        previous_solution = solution;

        solution =
            previous_solution + relaxation_coefficient * non_relaxed_update;
        function(solution, error);
        error -= solution;
        error_norm = tolerances.calc_norm(solution, error);
        while (error_norm > previous_error_norm) {
            constexpr double relaxation_coefficient_reduction_rate = 0.5;
            relaxation_coefficient *= relaxation_coefficient_reduction_rate;
            if (relaxation_coefficient <
                std::numeric_limits<double>::epsilon()) {
                NUM_COLLECT_LOG_AND_THROW(num_collect::algorithm_failure,
                    logger, "Failed to converge.");
            }

            solution =
                previous_solution + relaxation_coefficient * non_relaxed_update;
            function(solution, error);
            error -= solution;
            error_norm = tolerances.calc_norm(solution, error);
        }
        constexpr double relaxation_coefficient_increase_rate = 1.05;
        relaxation_coefficient *= relaxation_coefficient_increase_rate;
        error_reduction_rate = error_norm / previous_error_norm;

        ++iterations;
        iteration_logger.write_iteration();

        constexpr double tolerance_rate = 1e-4;
        if (error_norm <= tolerance_rate) {
            break;
        }
        previous_error_norm = error_norm;
        non_relaxed_update = error;
    }
    iteration_logger.write_summary();
}

/*!
 * \brief Solve for a time step with fixed-point iteration.
 *
 * \param[in] problem Problem.
 * \param[in] time_step_size Time step size.
 * \param[in] time Current time.
 * \param[in,out] solution Solution vector.
 * \param[in] relaxation_coefficient Relaxation coefficient for the fixed-point
 * iteration.
 */
static void step_with_fixed_point_iteration(ode_problem_type& problem,
    double time_step_size, double& time, solution_type& solution,
    double relaxation_coefficient) {
    constexpr double slope_coeff =
        0.5;  // Coefficient of slope in Crank-Nicolson.

    num_collect::ode::error_tolerances<solution_type> tolerances;

    problem.evaluate_on(
        time, solution, num_collect::ode::evaluation_type{.diff_coeff = true});
    const solution_type k1 = problem.diff_coeff();

    // Initial guess using the result of the first stage.
    const solution_type solution_offset = time_step_size * slope_coeff * k1;
    solution_type z2 = solution_offset;
    solution_type temp_variable;

    auto function = [&](const solution_type& input, solution_type& output) {
        temp_variable = solution + input;
        problem.evaluate_on(time + time_step_size, temp_variable,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        output = time_step_size * slope_coeff * problem.diff_coeff() +
            solution_offset;
    };

    fixed_point_iteration(function, z2, relaxation_coefficient, tolerances);

    solution += z2;
    time += time_step_size;
}

/*!
 * \brief Solve for a time step and verify the solution.
 *
 * \param[in] problem Problem.
 * \param[in] time_step_size Time step size.
 * \param[in] diffusion_coefficient Diffusion coefficient.
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] equation_solver_type_str Type of the equation solver.
 * \param[in] relaxation_coefficient Relaxation coefficient for the fixed-point
 * iteration.
 */
static void perform_test(ode_problem_type& problem, double time_step_size,
    double diffusion_coefficient,
    num_collect::util::vector_view<const position_type> nodes,
    num_collect::index_type num_interior_nodes,
    const std::string& equation_solver_type_str,
    double relaxation_coefficient) {
    num_collect::logging::logger logger;

    double time = 0.0;
    solution_type solution = solution_type::Zero(num_interior_nodes);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        solution(i) = test_function(nodes[i], time, diffusion_coefficient);
    }

    const auto start_time = std::chrono::steady_clock::now();
    if (equation_solver_type_str == "inexact_newton") {
        step_with_inexact_newton(problem, time_step_size, time, solution);
    } else if (equation_solver_type_str == "fixed_point_iteration") {
        step_with_fixed_point_iteration(
            problem, time_step_size, time, solution, relaxation_coefficient);
    } else {
        NUM_COLLECT_LOG_AND_THROW(num_collect::invalid_argument, logger,
            "Invalid equation solver type: {}", equation_solver_type_str);
    }
    const auto end_time = std::chrono::steady_clock::now();
    const auto elapsed_time_ms =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            end_time - start_time)
            .count();
    NUM_COLLECT_LOG_INFO(
        logger, "Elapsed time for the step: {:.5f} ms", elapsed_time_ms);

    solution_type true_values = solution_type::Zero(num_interior_nodes);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        true_values(i) = test_function(nodes[i], time, diffusion_coefficient);
    }
    const solution_type errors = (solution - true_values).cwiseAbs();

    const double max_error = errors.maxCoeff();
    const double mean_error = errors.head(num_interior_nodes).mean();
    const double max_value = true_values.cwiseAbs().maxCoeff();
    NUM_COLLECT_LOG_INFO(logger,
        "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
        "{:.2e}",
        time, max_error, mean_error, max_value);
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/runge_kutta_equation_solver.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    toml_parser parser(config_file_path);
    const auto num_interior_nodes = parser.get<num_collect::index_type>(
        "runge_kutta_equation_solver.num_interior_nodes");
    const auto num_boundary_nodes_per_edge =
        parser.get<num_collect::index_type>(
            "runge_kutta_equation_solver.num_boundary_nodes_per_edge");
    const auto polynomial_order =
        parser.get<int>("runge_kutta_equation_solver.polynomial_order");
    const auto num_neighbors = parser.get<num_collect::index_type>(
        "runge_kutta_equation_solver.num_neighbors");
    const auto diffusion_coefficient =
        parser.get<double>("runge_kutta_equation_solver.diffusion_coefficient");
    const auto time_step_size =
        parser.get<double>("runge_kutta_equation_solver.time_step_size");
    const auto equation_solver_type_str = parser.get<std::string>(
        "runge_kutta_equation_solver.equation_solver_type");
    const auto relaxation_coefficient = parser.get<double>(
        "runge_kutta_equation_solver.relaxation_coefficient");
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Number of boundary nodes per edge: {}",
        num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(
        logger, "Diffusion coefficient: {}", diffusion_coefficient);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(
        logger, "Equation solver type: {}", equation_solver_type_str);
    NUM_COLLECT_LOG_INFO(
        logger, "Relaxation coefficient: {}", relaxation_coefficient);

    const auto nodes =
        generate_nodes(num_interior_nodes, num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    auto problem = assemble_system(nodes, num_interior_nodes, polynomial_order,
        num_neighbors, diffusion_coefficient);

    perform_test(problem, time_step_size, diffusion_coefficient, nodes,
        num_interior_nodes, equation_solver_type_str, relaxation_coefficient);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
