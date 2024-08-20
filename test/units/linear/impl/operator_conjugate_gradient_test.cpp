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
 * \brief Test of operator_conjugate_gradient class.
 */
#include "num_collect/linear/impl/operator_conjugate_gradient.h"

#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "eigen_approx.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

TEST_CASE("num_collect::linear::impl::operator_conjugate_gradient") {
    using num_collect::linear::impl::operator_conjugate_gradient;
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

    operator_conjugate_gradient<vector_type> solver;

    SECTION("solve") {
        vector_type solution = vector_type::Zero(true_sol.size());
        solver.solve([&grid](const vector_type& target,
                         vector_type& result) { result = grid.mat() * target; },
            right, solution);

        constexpr double tolerance = 1e-10;
        CHECK_THAT(solution, eigen_approx(true_sol, tolerance));
    }
}
