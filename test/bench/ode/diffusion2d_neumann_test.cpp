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
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/ode/rosenbrock/ros34prw_formula.h"
#include "num_collect/ode/rosenbrock/ros34pw3_formula.h"
#include "num_collect/ode/runge_kutta/lobatto3c4_formula.h"
#include "num_collect/ode/runge_kutta/lobatto3c6_formula.h"
#include "num_collect/ode/runge_kutta/radau2a3_formula.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_collect/ode/runge_kutta/radau2a_formula.h"
#include "num_collect/ode/runge_kutta/sdirk4_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/operators/partial_derivative_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

using position_type = Eigen::Vector2d;
using solution_type = Eigen::VectorXd;
using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
using problem_type =
    num_collect::ode::problems::linear_first_order_dae_problem<solution_type,
        sparse_matrix_type>;

static constexpr std::string_view problem_name = "diffusion2d_neumann";
static constexpr std::string_view problem_description =
    "2D Diffusion Equation with Neumann Boundary Conditions";

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

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr num_collect::index_type num_interior_nodes = 25;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 5;
#else
    constexpr num_collect::index_type num_interior_nodes = 2500;
    constexpr num_collect::index_type num_boundary_nodes_per_edge = 50;
#endif
    auto nodes =
        num_collect::rbf::generate_halton_nodes<typename position_type::Scalar,
            position_type::RowsAtCompileTime>(num_interior_nodes);
    for (auto& node : nodes) {
        node.y() *= 0.5;
    }
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            position_type(0.0, 0.0), position_type(1.0, 0.5),
            num_boundary_nodes_per_edge);
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    const auto interior_nodes =
        num_collect::util::vector_view<const position_type>(nodes).first(
            num_interior_nodes);

    const num_collect::util::nearest_neighbor_searcher<position_type>
        column_variables_nearest_neighbor_searcher(nodes);

    constexpr num_collect::index_type num_neighbors = 15;
    num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
    stiffness_triplets.reserve(nodes.size() * num_neighbors);
    num_collect::util::vector<Eigen::Triplet<double>> mass_triplets;
    mass_triplets.reserve(nodes.size());
    Eigen::VectorXd load_vector = Eigen::VectorXd::Zero(nodes.size());

    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler;
    assembler.num_neighbors(num_neighbors);

    // Equations for interior nodes.
    {
        using operator_type =
            num_collect::rbf::operators::laplacian_operator<position_type>;
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows(
            [](const position_type& position) {
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
        const auto neumann_boundary_nodes =
            num_collect::util::vector_view<const position_type>(nodes).subview(
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

    const problem_type problem(mass_matrix, stiffness_matrix, load_vector);

    solution_type initial_solution = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        initial_solution(i) = test_function(nodes[i], initial_time);
    }

    solution_type true_values = solution_type::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
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

    bench_one<num_collect::ode::runge_kutta::sdirk4_solver<problem_type>>(
        "SDIRK4", executor);
    bench_one<
        num_collect::ode::runge_kutta::lobatto3c4_auto_solver<problem_type>>(
        "LobattoIIIC4", executor);
    bench_one<
        num_collect::ode::runge_kutta::lobatto3c6_auto_solver<problem_type>>(
        "LobattoIIIC6", executor);
    bench_one<
        num_collect::ode::runge_kutta::radau2a3_auto_solver<problem_type>>(
        "RadauIIA3", executor);
    bench_one<
        num_collect::ode::runge_kutta::radau2a5_auto_solver<problem_type>>(
        "RadauIIA5", executor);
    bench_one<
        num_collect::ode::runge_kutta::radau2a9_auto_solver<problem_type>>(
        "RadauIIA9", executor);
    bench_one<
        num_collect::ode::runge_kutta::radau2a13_auto_solver<problem_type>>(
        "RadauIIA13", executor);
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
