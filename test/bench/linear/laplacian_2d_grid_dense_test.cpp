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
#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/LU>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/memory_barrier.h>

#include "laplacian_2d_grid_make_sol.h"
#include "num_collect/base/index_type.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

class laplacian_2d_grid_dense_fixture : public stat_bench::FixtureBase {
public:
    laplacian_2d_grid_dense_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(4 * 4)    // NOLINT
            ->add(10 * 10)  // NOLINT
#ifdef NDEBUG
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

STAT_BENCH_CASE_F(laplacian_2d_grid_dense_fixture, "laplacian_2d_grid", "LLT") {
    using mat_type = Eigen::SparseMatrix<double>;

    num_prob_collect::linear::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    const Eigen::MatrixXd dense_mat = grid.mat();
    Eigen::LLT<Eigen::MatrixXd> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(dense_mat);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(
    laplacian_2d_grid_dense_fixture, "laplacian_2d_grid", "LDLT") {
    using mat_type = Eigen::SparseMatrix<double>;

    num_prob_collect::linear::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    const Eigen::MatrixXd dense_mat = grid.mat();
    Eigen::LDLT<Eigen::MatrixXd> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(dense_mat);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_residual(grid.mat(), sol, right);
}

STAT_BENCH_CASE_F(
    laplacian_2d_grid_dense_fixture, "laplacian_2d_grid", "PartialPivLU") {
    using mat_type = Eigen::SparseMatrix<double>;

    num_prob_collect::linear::laplacian_2d_grid<mat_type> grid{
        grid_rows(), grid_cols(), grid_width()};
    const Eigen::VectorXd true_sol = laplacian_2d_grid_make_sol(grid);
    const Eigen::VectorXd right = grid.mat() * true_sol;
    const Eigen::MatrixXd dense_mat = grid.mat();
    Eigen::PartialPivLU<Eigen::MatrixXd> solver;
    Eigen::VectorXd sol;

    STAT_BENCH_MEASURE() {
        solver.compute(dense_mat);
        sol = solver.solve(right);
        stat_bench::memory_barrier();
    };

    set_residual(grid.mat(), sol, right);
}
