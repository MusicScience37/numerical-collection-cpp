/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Benchmark of solving equations of Laplacian matrices.
 */
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/OrderingMethods>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/current_invocation_context.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/memory_barrier.h>

#include "laplacian_2d_grid_make_sol.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/gauss_seidel_iterative_solver.h"
#include "num_collect/linear/symmetric_successive_over_relaxation.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

class laplacian_2d_grid_iterative_fixture : public stat_bench::FixtureBase {
public:
    laplacian_2d_grid_iterative_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(4 * 4)    // NOLINT
            ->add(10 * 10)  // NOLINT
#ifdef NDEBUG
            ->add(32 * 32)    // NOLINT
            ->add(100 * 100)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<num_collect::index_type>("size");
        grid_rows_ = static_cast<num_collect::index_type>(std::sqrt(size_));
        grid_width_ = 1.0 / static_cast<double>(grid_rows_ + 1);
    }

    [[nodiscard]] auto mat_size() const noexcept -> num_collect::index_type {
        return size_;
    }

    [[nodiscard]] auto grid_rows() const noexcept -> num_collect::index_type {
        return grid_rows_;
    }

    [[nodiscard]] auto grid_cols() const noexcept -> num_collect::index_type {
        return grid_rows_;
    }

    [[nodiscard]] auto grid_width() const noexcept -> double {
        return grid_width_;
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
    num_collect::index_type grid_rows_{};
    double grid_width_{};
};

class laplacian_2d_grid_iterative_slower_fixture
    : public stat_bench::FixtureBase {
public:
    laplacian_2d_grid_iterative_slower_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(4 * 4)  // NOLINT
#ifdef NDEBUG
            ->add(10 * 10)  // NOLINT
            ->add(32 * 32)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<num_collect::index_type>("size");
        grid_rows_ = static_cast<num_collect::index_type>(std::sqrt(size_));
        grid_width_ = 1.0 / static_cast<double>(grid_rows_ + 1);
    }

    [[nodiscard]] auto mat_size() const noexcept -> num_collect::index_type {
        return size_;
    }

    [[nodiscard]] auto grid_rows() const noexcept -> num_collect::index_type {
        return grid_rows_;
    }

    [[nodiscard]] auto grid_cols() const noexcept -> num_collect::index_type {
        return grid_rows_;
    }

    [[nodiscard]] auto grid_width() const noexcept -> double {
        return grid_width_;
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
    num_collect::index_type grid_rows_{};
    double grid_width_{};
};

STAT_BENCH_CASE_F(
    laplacian_2d_grid_iterative_fixture, "laplacian_2d_grid", "CG") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    num_prob_collect::finite_element::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    Eigen::ConjugateGradient<mat_type, Eigen::Upper | Eigen::Lower> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(grid.mat());
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(
    laplacian_2d_grid_iterative_fixture, "laplacian_2d_grid", "ICCG") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    num_prob_collect::finite_element::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    Eigen::ConjugateGradient<mat_type, Eigen::Upper | Eigen::Lower,
        Eigen::IncompleteCholesky<double>>
        solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(grid.mat());
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(
    laplacian_2d_grid_iterative_fixture, "laplacian_2d_grid", "BiCGstab") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    num_prob_collect::finite_element::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    Eigen::BiCGSTAB<mat_type> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(grid.mat());
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(laplacian_2d_grid_iterative_slower_fixture,
    "laplacian_2d_grid", "Gauss-Seidel") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    num_prob_collect::finite_element::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    num_collect::linear::gauss_seidel_iterative_solver<mat_type> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(grid.mat());
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(laplacian_2d_grid_iterative_slower_fixture,
    "laplacian_2d_grid", "SSOR(0.5)") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    num_prob_collect::finite_element::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    num_collect::linear::symmetric_successive_over_relaxation<mat_type> solver;
    Eigen::VectorXd sol;

    solver.relaxation_coeff(0.5);  // NOLINT

    STAT_BENCH_MEASURE() {
        solver.compute(grid.mat());
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(laplacian_2d_grid_iterative_slower_fixture,
    "laplacian_2d_grid", "SSOR(1.0)") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    num_prob_collect::finite_element::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    num_collect::linear::symmetric_successive_over_relaxation<mat_type> solver;
    Eigen::VectorXd sol;

    solver.relaxation_coeff(1.0);  // NOLINT

    STAT_BENCH_MEASURE() {
        solver.compute(grid.mat());
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(laplacian_2d_grid_iterative_slower_fixture,
    "laplacian_2d_grid", "SSOR(1.5)") {
    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

    num_prob_collect::finite_element::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    num_collect::linear::symmetric_successive_over_relaxation<mat_type> solver;
    Eigen::VectorXd sol;

    solver.relaxation_coeff(1.5);  // NOLINT

    STAT_BENCH_MEASURE() {
        solver.compute(grid.mat());
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_iterations(solver.iterations());
    set_residual(grid.mat(), sol, right);
}
