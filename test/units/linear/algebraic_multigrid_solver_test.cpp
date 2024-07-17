/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of algebraic_multigrid_solver class.
 */
#include "num_collect/linear/algebraic_multigrid_solver.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

TEST_CASE("num_collect::linear::algebraic_multigrid_solver") {
    using num_collect::linear::algebraic_multigrid_solver;
    using num_prob_collect::linear::laplacian_2d_grid;

    using scalar_type = double;
    using vector_type = Eigen::VectorX<scalar_type>;
    using matrix_type = Eigen::SparseMatrix<scalar_type, Eigen::RowMajor>;

    constexpr num_collect::index_type grid_size = 10;
    constexpr auto grid_width = static_cast<scalar_type>(0.1);
    laplacian_2d_grid<matrix_type> grid{grid_size, grid_size, grid_width};

    vector_type true_sol(grid_size * grid_size);
    for (num_collect::index_type i = 0; i < grid_size; ++i) {
        const scalar_type x = grid_width * static_cast<scalar_type>(i);
        for (num_collect::index_type j = 0; j < grid_size; ++j) {
            const scalar_type y = grid_width * static_cast<scalar_type>(j);
            const scalar_type val = x * x + y * y;
            true_sol(i + grid_size * j) = val;
        }
    }

    const vector_type right = grid.mat() * true_sol;

    algebraic_multigrid_solver<matrix_type> solver;

    // At least one layer should exist for tests.
    constexpr num_collect::index_type maximum_directly_solved_matrix_size = 10;
    solver.maximum_directly_solved_matrix_size(
        maximum_directly_solved_matrix_size);

    SECTION("iterate only once") {
        solver.compute(grid.mat());
        const scalar_type res0 = right.squaredNorm();

        solver.max_iterations(1);
        const vector_type sol = solver.solve(right);

        CHECK(solver.iterations() == 1);
        const scalar_type res1 = (grid.mat() * sol - right).squaredNorm();
        CHECK(res1 < res0);
    }

    SECTION("solve") {
        solver.compute(grid.mat());

        const vector_type sol = solver.solve(right);

        const scalar_type res_rate =
            (grid.mat() * sol - right).norm() / right.norm();
        CHECK(res_rate < Eigen::NumTraits<scalar_type>::dummy_precision());
        CHECK(solver.iterations() > 1);
    }

    SECTION("solve with guess") {
        solver.compute(grid.mat());

        const vector_type sol = solver.solve_with_guess(right, true_sol);

        const scalar_type res_rate =
            (grid.mat() * sol - right).norm() / right.norm();
        CHECK(res_rate < Eigen::NumTraits<scalar_type>::dummy_precision());
        CHECK(solver.iterations() == 1);
    }
}
