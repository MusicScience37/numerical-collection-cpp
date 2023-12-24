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
 * \brief Test to solve Laplacian equation in 2-dimensional grid.
 */
#include "num_prob_collect/linear/laplacian_2d_grid.h"

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseCore>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"
#include "num_collect/linear/cuthill_mckee_ordering.h"
#include "num_collect/linear/gauss_seidel_iterative_solver.h"
#include "num_collect/linear/reverse_cuthill_mckee_ordering.h"
#include "num_collect/linear/symmetric_successive_over_relaxation.h"
#include "num_collect/logging/logger.h"

TEMPLATE_TEST_CASE("Solver Laplacian equation in 2-dimensional grid", "",
    (Eigen::ConjugateGradient<Eigen::SparseMatrix<double>,
        Eigen::Upper | Eigen::Lower>),
    (Eigen::ConjugateGradient<Eigen::SparseMatrix<double>,
        Eigen::Upper | Eigen::Lower, Eigen::IncompleteCholesky<double>>),
    (Eigen::ConjugateGradient<Eigen::SparseMatrix<double>,
        Eigen::Upper | Eigen::Lower,
        Eigen::IncompleteCholesky<double, Eigen::Lower,
            num_collect::linear::cuthill_mckee_ordering<int>>>),
    (Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>, Eigen::Lower>),
    (Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>, Eigen::Lower,
        num_collect::linear::cuthill_mckee_ordering<int>>),
    (num_collect::linear::gauss_seidel_iterative_solver<
        Eigen::SparseMatrix<double, Eigen::RowMajor>>),
    (num_collect::linear::symmetric_successive_over_relaxation<
        Eigen::SparseMatrix<double, Eigen::RowMajor>>)) {
    using solver_type = TestType;
    using mat_type = typename solver_type::MatrixType;
    using vec_type = Eigen::VectorXd;
    using grid_type =
        num_prob_collect::finite_element::laplacian_2d_grid<mat_type>;

    constexpr double region_size = 1.0;
    constexpr num_collect::index_type grid_size = 5;

    const auto expected_function = [](double x, double y) {
        return x * x + y * y;
    };

    num_collect::logging::logger logger;

    const double grid_width = region_size / static_cast<double>(grid_size);
    grid_type grid{grid_size - 1, grid_size - 1, grid_width};
    vec_type expected_solution(grid.mat_size());
    for (num_collect::index_type xi = 0; xi < grid_size - 1; ++xi) {
        const double x =
            static_cast<double>(xi + 1) / static_cast<double>(grid_size);
        for (num_collect::index_type yi = 0; yi < grid_size - 1; ++yi) {
            const double y =
                static_cast<double>(yi + 1) / static_cast<double>(grid_size);
            const num_collect::index_type vector_index = grid.index(xi, yi);
            expected_solution(vector_index) = expected_function(x, y);
        }
    }
    const vec_type right_vector = grid.mat() * expected_solution;

    SECTION("solve a problem") {
        solver_type solver;
        solver.compute(grid.mat());
        const vec_type solution = solver.solve(right_vector);

        const double max_error =
            (solution - expected_solution).cwiseAbs().maxCoeff();
        logger.info()("Maximum error: {}", max_error);
        constexpr double threshold = 1e-10;
        CHECK(max_error < threshold);
    }

    SECTION("use cuthill_mckee_ordering") {
        num_collect::linear::cuthill_mckee_ordering<int> ordering;
        Eigen::PermutationMatrix<Eigen::Dynamic> permutation;
        ordering(grid.mat(), permutation);
        mat_type ordered_matrix;
        ordered_matrix = grid.mat().twistedBy(permutation);
        const vec_type ordered_right_vector = permutation * right_vector;

        solver_type solver;
        solver.compute(ordered_matrix);
        const vec_type ordered_solution = solver.solve(ordered_right_vector);
        const vec_type solution = permutation.inverse() * ordered_solution;

        const double max_error =
            (solution - expected_solution).cwiseAbs().maxCoeff();
        logger.info()("Maximum error: {}", max_error);
        constexpr double threshold = 1e-10;
        CHECK(max_error < threshold);
    }

    SECTION("use reverse_cuthill_mckee_ordering") {
        num_collect::linear::reverse_cuthill_mckee_ordering<int> ordering;
        Eigen::PermutationMatrix<Eigen::Dynamic> permutation;
        ordering(grid.mat(), permutation);
        mat_type ordered_matrix;
        ordered_matrix = grid.mat().twistedBy(permutation);
        const vec_type ordered_right_vector = permutation * right_vector;

        solver_type solver;
        solver.compute(ordered_matrix);
        const vec_type ordered_solution = solver.solve(ordered_right_vector);
        const vec_type solution = permutation.inverse() * ordered_solution;

        const double max_error =
            (solution - expected_solution).cwiseAbs().maxCoeff();
        logger.info()("Maximum error: {}", max_error);
        constexpr double threshold = 1e-10;
        CHECK(max_error < threshold);
    }
}
