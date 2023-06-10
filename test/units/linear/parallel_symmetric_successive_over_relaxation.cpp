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
 * \brief Definition of parallel_symmetric_successive_over_relaxation class.
 */
#include "num_collect/linear/parallel_symmetric_successive_over_relaxation.h"

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

TEMPLATE_TEST_CASE(
    "num_collect::linear::parallel_symmetric_successive_over_relaxation", "",
    float, double, long double) {
    using num_collect::linear::parallel_symmetric_successive_over_relaxation;
    using num_prob_collect::finite_element::laplacian_2d_grid;

    using scalar_type = TestType;
    using vector_type = Eigen::VectorX<scalar_type>;
    using matrix_type = Eigen::SparseMatrix<scalar_type, Eigen::RowMajor>;

    constexpr num_collect::index_type grid_size = 3;
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

    parallel_symmetric_successive_over_relaxation<matrix_type> solver;

    SECTION("iterate only once (parallel)") {
        solver.compute(grid.mat());
        solver.run_parallel(true);
        const scalar_type res0 = right.squaredNorm();

        solver.max_iterations(1);
        const vector_type sol = solver.solve(right);

        CHECK(solver.iterations() == 1);
        const scalar_type res1 = (grid.mat() * sol - right).squaredNorm();
        CHECK(res1 < res0);
    }

    SECTION("iterate only once (parallel)") {
        solver.compute(grid.mat());
        solver.run_parallel(false);
        const scalar_type res0 = right.squaredNorm();

        solver.max_iterations(1);
        const vector_type sol = solver.solve(right);

        CHECK(solver.iterations() == 1);
        const scalar_type res1 = (grid.mat() * sol - right).squaredNorm();
        CHECK(res1 < res0);
    }

    SECTION("solve (parallel)") {
        solver.compute(grid.mat());
        solver.run_parallel(true);

        const vector_type sol = solver.solve(right);

        const scalar_type res_rate =
            (grid.mat() * sol - right).norm() / right.norm();
        CHECK(res_rate < Eigen::NumTraits<scalar_type>::dummy_precision());
        CHECK(solver.iterations() > 1);
    }

    SECTION("solve (not parallel)") {
        solver.compute(grid.mat());
        solver.run_parallel(false);

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

    SECTION("try to use invalid coefficients") {
        matrix_type custom_mat = grid.mat();
        custom_mat.coeffRef(1, 1) = static_cast<scalar_type>(0);

        CHECK_THROWS(solver.compute(custom_mat));
    }
}
