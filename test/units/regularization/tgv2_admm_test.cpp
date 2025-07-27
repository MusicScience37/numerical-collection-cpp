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
 * \brief Test of tgv2_admm class.
 */
#include "num_collect/regularization/tgv2_admm.h"

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "num_prob_collect/regularization/sparse_blur_matrix.h"
#include "num_prob_collect/regularization/sparse_diff_matrix.h"

TEST_CASE("num_collect::regularization::tgv2_admm") {
    using num_collect::regularization::tgv2_admm;
    using num_prob_collect::regularization::sparse_blur_matrix;
    using num_prob_collect::regularization::sparse_diff_matrix;

    using scalar_type = double;
    using coeff_type = Eigen::SparseMatrix<scalar_type>;
    using derivative_matrix_type = Eigen::SparseMatrix<scalar_type>;
    using data_type = Eigen::VectorX<scalar_type>;
    using solver_type =
        tgv2_admm<coeff_type, derivative_matrix_type, data_type>;

    SECTION("solve 1D problem") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 12;

        coeff_type coeff;
        sparse_blur_matrix(coeff, data_size, solution_size);
        const data_type true_solution{{
            0.0, 0.4, 0.8, 0.4, 0.0,   //
            0.0, 0.0, 0.0, 0.0, -0.2,  //
            -0.2, 0.0, 0.0, 0.0, 0.0   //
        }};
        const data_type data = coeff * true_solution;
        REQUIRE(true_solution.rows() == solution_size);
        REQUIRE(data.rows() == data_size);

        const auto derivative_matrix =
            sparse_diff_matrix<derivative_matrix_type>(solution_size);
        const auto divergence_matrix =
            sparse_diff_matrix<derivative_matrix_type>(solution_size - 1);

        solver_type solver;
        REQUIRE_NOTHROW(
            solver.compute(coeff, derivative_matrix, divergence_matrix, data));

        constexpr scalar_type param = 0.5e-2;
        data_type solution = data_type::Zero(solution_size);

        SECTION("iterate once") {
            REQUIRE_NOTHROW(solver.init(param, solution));

            REQUIRE_NOTHROW(solver.iterate(param, solution));
        }

        SECTION("solve") {
            REQUIRE_NOTHROW(solver.solve(param, solution));

            constexpr num_collect::index_type precision = 4;
            comparison_approvals::verify_with_reference(
                solution, true_solution, precision);
        }
    }
}
