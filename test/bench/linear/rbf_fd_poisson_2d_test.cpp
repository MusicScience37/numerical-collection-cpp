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
 * \brief Benchmark of linear solvers using Poisson equation in 2D discretized
 * using RBF-FD.
 */
#include <cmath>
#include <utility>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>

#include "num_collect/base/concepts/sparse_matrix_of.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

template <num_collect::concepts::sparse_matrix_of<double> Matrix>
[[nodiscard]] auto generate_problem(num_collect::index_type num_interior_nodes)
    -> std::pair<Matrix, Eigen::VectorXd> {
    using variable_type = Eigen::Vector2d;
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<variable_type>;

    auto test_function = [](const variable_type& variable) -> double {
        return (variable.array() * num_collect::base::pi<double>).sin().prod();
    };
    auto test_function_laplacian = [](const variable_type& variable) -> double {
        constexpr double coeff = -2.0 * num_collect::base::pi<double> *
            num_collect::base::pi<double>;
        return coeff *
            (variable.array() * num_collect::base::pi<double>).sin().prod();
    };

    const auto num_boundary_nodes_per_edge =
        static_cast<num_collect::index_type>(std::sqrt(num_interior_nodes));

    auto nodes =
        num_collect::rbf::generate_halton_nodes<typename variable_type::Scalar,
            variable_type::RowsAtCompileTime>(num_interior_nodes);
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            variable_type(0.0, 0.0), variable_type(1.0, 1.0),
            num_boundary_nodes_per_edge);
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    const auto interior_nodes =
        num_collect::util::vector_view<const variable_type>(nodes).first(
            num_interior_nodes);

    assembler_type assembler;
    assembler.num_neighbors(15);
    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd right_vec(nodes.size());
    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);
    assembler.compute_rows<operator_type>(interior_nodes, nodes,
        column_variables_nearest_neighbor_searcher, triplets, 0, 0);
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        right_vec(i) = test_function_laplacian(interior_nodes[i]);
    }
    for (num_collect::index_type i = num_interior_nodes; i < nodes.size();
        ++i) {
        const auto index_in_triplet = static_cast<int>(i);
        triplets.emplace_back(index_in_triplet, index_in_triplet, 1.0);
        right_vec(i) = test_function(nodes[i]);
    }
    Matrix mat(nodes.size(), nodes.size());
    mat.setFromTriplets(triplets.begin(), triplets.end());

    return {std::move(mat), std::move(right_vec)};
}

class rbf_fd_poisson_2d_common_fixture : public stat_bench::FixtureBase {
public:
    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<num_collect::index_type>("size");
    }

    [[nodiscard]] auto size() const noexcept -> num_collect::index_type {
        return size_;
    }

    static void set_iterations(num_collect::index_type iterations) {
        stat_bench::current_invocation_context().add_custom_output(
            "iterations", static_cast<double>(iterations));
    }

    template <typename Matrix, typename Vector>
    static void set_residual(
        const Matrix& coeff, const Vector& solution, const Vector& right) {
        stat_bench::current_invocation_context().add_custom_output(
            "residual", (coeff * solution - right).norm() / right.norm());
    }

private:
    num_collect::index_type size_{};
};

class rbf_fd_poisson_2d_large_fixture
    : public rbf_fd_poisson_2d_common_fixture {
public:
    rbf_fd_poisson_2d_large_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(100)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(300)
            ->add(1000)
            ->add(3000)
            ->add(10000)
            ->add(30000)
#endif
            ;
    }
};

class rbf_fd_poisson_2d_middle_fixture
    : public rbf_fd_poisson_2d_common_fixture {
public:
    rbf_fd_poisson_2d_middle_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(100)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(300)
            ->add(1000)
            ->add(3000)
            ->add(10000)
#endif
            ;
    }
};

class rbf_fd_poisson_2d_small_fixture
    : public rbf_fd_poisson_2d_common_fixture {
public:
    rbf_fd_poisson_2d_small_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(100)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(300)
            ->add(1000)
            ->add(3000)
#endif
            ;
    }
};

constexpr double tolerance = 1e-6;

STAT_BENCH_CASE_F(
    rbf_fd_poisson_2d_large_fixture, "rbf_fd_poisson_2d", "BiCGstab") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    const auto [matrix, right] = generate_problem<mat_type>(size());
    Eigen::BiCGSTAB<mat_type> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.setTolerance(tolerance);
        solver.compute(matrix);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(matrix, sol, right);
}

STAT_BENCH_CASE_F(
    rbf_fd_poisson_2d_large_fixture, "rbf_fd_poisson_2d", "BiCGstab-ILU") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    const auto [matrix, right] = generate_problem<mat_type>(size());
    Eigen::BiCGSTAB<mat_type, Eigen::IncompleteLUT<double>> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.setTolerance(tolerance);
        solver.compute(matrix);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(matrix, sol, right);
}

STAT_BENCH_CASE_F(
    rbf_fd_poisson_2d_small_fixture, "rbf_fd_poisson_2d", "PartialPivLU") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::ColMajor>;

    const auto [sparse_matrix, right] = generate_problem<mat_type>(size());
    const Eigen::MatrixXd matrix = sparse_matrix;
    Eigen::PartialPivLU<Eigen::MatrixXd> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(matrix);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(1);
    set_residual(matrix, sol, right);
}

STAT_BENCH_CASE_F(
    rbf_fd_poisson_2d_large_fixture, "rbf_fd_poisson_2d", "SparseLU") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::ColMajor>;

    const auto [matrix, right] = generate_problem<mat_type>(size());
    Eigen::SparseLU<mat_type> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(matrix);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(1);
    set_residual(matrix, sol, right);
}

STAT_BENCH_GROUP("rbf_fd_poisson_2d")
    .add_parameter_to_time_line_plot(
        "size", stat_bench::PlotOptions().log_parameter(true))
    .clear_measurement_configs()
    .add_measurement_config(stat_bench::MeasurementConfig()
            .type("Processing Time")
            .iterations(1)
            .samples(10)
            .warming_up_samples(1));

STAT_BENCH_MAIN
