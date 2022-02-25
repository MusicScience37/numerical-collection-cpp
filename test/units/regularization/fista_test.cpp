/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of fista class.
 */
#include "num_collect/regularization/fista.h"

#include <ApprovalTests.hpp>
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/util/format_dense_vector.h"
#include "num_prob_collect/regularization/sparse_blur_matrix.h"

TEST_CASE("num_collect::regularization::fista") {
    using num_collect::regularization::fista;
    using num_prob_collect::regularization::sparse_blur_matrix;

    using scalar_type = double;
    using coeff_type = Eigen::SparseMatrix<scalar_type>;
    using data_type = Eigen::VectorX<scalar_type>;
    using solver_type = fista<coeff_type, data_type>;

    SECTION("solve") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 15;

        coeff_type coeff;
        sparse_blur_matrix(coeff, data_size, solution_size);
        const data_type true_solution{{
            0.0, 0.8, 0.8, 0.8, 0.0,   //
            0.0, 0.0, 0.0, 0.0, -0.2,  //
            -0.2, 0.0, 0.0, 0.0, 0.0   //
        }};
        const data_type data = coeff * true_solution;
        REQUIRE(true_solution.rows() == solution_size);
        REQUIRE(data.rows() == data_size);

        solver_type solver;
        REQUIRE_NOTHROW(solver.compute(coeff, data));

        REQUIRE_NOTHROW((void)solver.param_search_region());

        constexpr scalar_type param = 1e-2;
        data_type solution = data_type::Zero(solution_size);
        REQUIRE_NOTHROW(solver.solve(param, solution));

        ApprovalTests::Approvals::verify(fmt::format(
            "{:> 5.2f}", num_collect::util::format_dense_vector(solution)));
    }
}
