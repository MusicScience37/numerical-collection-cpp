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
 * \brief Benchmark of ODE solver for 2D diffusion equation.
 */
#include <cmath>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "diagram_common.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/ode/rosenbrock/ros34prw_formula.h"
#include "num_collect/ode/rosenbrock/ros34pw3_formula.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"
#include "num_prob_collect/ode/no_jacobian_linear_first_order_ode_problem.h"

using position_type = Eigen::Vector2d;
using solution_type = Eigen::VectorXd;
using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
using problem_type =
    num_prob_collect::ode::no_jacobian_linear_first_order_ode_problem<
        solution_type, sparse_matrix_type>;

static constexpr std::string_view problem_name =
    "no_jacobian_diffusion2d_dirichlet";
static constexpr std::string_view problem_description =
    "2D Diffusion Equation with Dirichlet Boundary Conditions Without Jacobian";

static constexpr double diffusion_coefficient = 0.1;

static auto test_function(const position_type& position, double time)
    -> double {
    return std::exp(-2.0 * diffusion_coefficient * num_collect::pi<double> *
               num_collect::pi<double> * time) *
        (position.array() * num_collect::pi<double>).sin().prod();
}

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, bench_executor& executor) {
    constexpr double initial_time = 0.0;
    constexpr double final_time = 1.0;

    num_collect::logging::logger logger(benchmark_tag);

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr num_collect::index_type num_interior_nodes = 100;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 10;
#else
    constexpr num_collect::index_type num_interior_nodes = 1000;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 30;
#endif
    auto nodes =
        num_collect::rbf::generate_halton_nodes<typename position_type::Scalar,
            position_type::RowsAtCompileTime>(num_interior_nodes);
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    const auto interior_nodes =
        num_collect::util::vector_view<const position_type>(nodes).first(
            num_interior_nodes);

    using operator_type =
        num_collect::rbf::operators::laplacian_operator<position_type>;
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    using ode_problem_type = problem_type;
    assembler_type assembler;
    assembler.num_neighbors(15);
    num_collect::util::vector<Eigen::Triplet<double>> triplets;

    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);
    constexpr num_collect::index_type row_offset = 0;
    constexpr num_collect::index_type column_offset = 0;
    assembler.compute_rows(
        [](const position_type& position) {
            return diffusion_coefficient * operator_type(position);
        },
        interior_nodes, nodes, column_variables_nearest_neighbor_searcher,
        triplets, row_offset, column_offset);

    sparse_matrix_type whole_coefficients(num_interior_nodes, nodes.size());
    whole_coefficients.setFromTriplets(triplets.begin(), triplets.end());

    const sparse_matrix_type variable_coefficients =
        whole_coefficients.leftCols(num_interior_nodes);
    const solution_type constant_term = solution_type::Zero(num_interior_nodes);
    const ode_problem_type problem(variable_coefficients, constant_term);

    solution_type initial_solution = solution_type::Zero(num_interior_nodes);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        initial_solution(i) = test_function(nodes[i], initial_time);
    }

    solution_type true_values = solution_type::Zero(num_interior_nodes);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        true_values(i) = test_function(nodes[i], final_time);
    }

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr num_collect::index_type repetitions = 1;
#else
    constexpr num_collect::index_type repetitions = 1;
#endif

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr std::array<double, 3> tolerance_list{1e-1, 1e-2, 1e-3};
#else
    constexpr std::array<double, 3> tolerance_list{1e-1, 1e-2, 1e-3};
#endif

    for (const double tol : tolerance_list) {
        executor.perform<problem_type, Solver>(solver_name, problem,
            initial_time, final_time, initial_solution, true_values,
            repetitions, tol);
    }
}

auto main(int argc, char** argv) -> int {
    if (argc != 2) {
        fmt::print("Usage: {} <output_directory>", argv[0]);
        return 1;
    }

    const std::string_view output_directory = argv[1];

    configure_logging();

    bench_executor executor{};

    bench_one<num_collect::ode::runge_kutta::rkf45_solver<problem_type>>(
        "RKF45", executor);
    bench_one<num_collect::ode::rosenbrock::ros3w_solver<problem_type>>(
        "ROS3w", executor);
    bench_one<num_collect::ode::rosenbrock::ros34prw_solver<problem_type>>(
        "ROS34PRw", executor);
    bench_one<num_collect::ode::rosenbrock::ros34pw3_solver<problem_type>>(
        "ROS34PW3", executor);
    bench_one<num_collect::ode::rosenbrock::rodasp_solver<problem_type>>(
        "RODASP", executor);
    bench_one<num_collect::ode::rosenbrock::rodaspr_solver<problem_type>>(
        "RODASPR", executor);

    executor.write_result(problem_name, problem_description, output_directory);

    return 0;
}
