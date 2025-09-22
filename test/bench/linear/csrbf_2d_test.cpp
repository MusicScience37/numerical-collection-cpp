/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Benchmark of linear solvers using CSRBF in 2D.
 */
#include <utility>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>
#include <Eigen/SparseLU>
#include <Eigen/SuperLUSupport>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>

#include "num_collect/base/concepts/sparse_matrix_of.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/compute_kernel_matrix.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/length_parameter_calculators/local_length_parameter_calculator.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

template <num_collect::concepts::sparse_matrix_of<double> Matrix>
[[nodiscard]] auto generate_problem(num_collect::index_type num_nodes)
    -> std::pair<Matrix, Eigen::VectorXd> {
    using distance_function_type =
        num_collect::rbf::distance_functions::euclidean_distance_function<
            Eigen::Vector2d>;
    using rbf_type = num_collect::rbf::rbfs::wendland_csrbf<double, 3, 1>;
    using length_parameter_calculator_type =
        num_collect::rbf::length_parameter_calculators::
            local_length_parameter_calculator<distance_function_type>;

    constexpr double length_parameter_scale = 5.0;

    const auto nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_nodes);
    const distance_function_type distance_function;
    const rbf_type rbf;
    length_parameter_calculator_type length_parameter_calculator;
    length_parameter_calculator.scale(length_parameter_scale);
    Matrix kernel_matrix;
    num_collect::rbf::compute_kernel_matrix(distance_function, rbf,
        length_parameter_calculator, nodes, kernel_matrix);

    Eigen::VectorXd right;
    right.resize(num_nodes);
    for (num_collect::index_type i = 0; i < num_nodes; ++i) {
        right[i] = nodes[i].squaredNorm();
    }

    return {std::move(kernel_matrix), std::move(right)};
}

class csrbf_2d_common_fixture : public stat_bench::FixtureBase {
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

class csrbf_2d_large_fixture : public csrbf_2d_common_fixture {
public:
    csrbf_2d_large_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(100)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(300)    // NOLINT
            ->add(1000)   // NOLINT
            ->add(3000)   // NOLINT
            ->add(10000)  // NOLINT
            ->add(30000)  // NOLINT
#endif
            ;
    }
};

class csrbf_2d_middle_fixture : public csrbf_2d_common_fixture {
public:
    csrbf_2d_middle_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(100)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(300)    // NOLINT
            ->add(1000)   // NOLINT
            ->add(3000)   // NOLINT
            ->add(10000)  // NOLINT
#endif
            ;
    }
};

class csrbf_2d_small_fixture : public csrbf_2d_common_fixture {
public:
    csrbf_2d_small_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(100)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(300)   // NOLINT
            ->add(1000)  // NOLINT
            ->add(3000)  // NOLINT
#endif
            ;
    }
};

constexpr double tolerance = 1e-6;

STAT_BENCH_CASE_F(csrbf_2d_large_fixture, "csrbf_2d", "BiCGstab") {
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

STAT_BENCH_CASE_F(csrbf_2d_middle_fixture, "csrbf_2d", "BiCGstab-ILU") {
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

STAT_BENCH_CASE_F(csrbf_2d_small_fixture, "csrbf_2d", "PartialPivLU") {
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

STAT_BENCH_CASE_F(csrbf_2d_middle_fixture, "csrbf_2d", "SparseLU") {
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

STAT_BENCH_CASE_F(csrbf_2d_middle_fixture, "csrbf_2d", "SuperLU") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::ColMajor>;

    const auto [matrix, right] = generate_problem<mat_type>(size());
    Eigen::SuperLU<mat_type> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(matrix);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(1);
    set_residual(matrix, sol, right);
}

STAT_BENCH_GROUP("csrbf_2d")
    .add_parameter_to_time_line_plot(
        "size", stat_bench::PlotOptions().log_parameter(true))
    .clear_measurement_configs()
    .add_measurement_config(stat_bench::MeasurementConfig()
            .type("Processing Time")
            .iterations(1)
            .samples(10)
            .warming_up_samples(1));

STAT_BENCH_MAIN
