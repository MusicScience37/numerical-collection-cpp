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
 * \brief Test of format_sparse_matrix function.
 */
#include "num_collect/util/format_sparse_matrix.h"

#include <string>
#include <vector>

#include <ApprovalTests.hpp>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::format_sparse_matrix") {
    using num_collect::util::format_sparse_matrix;
    using num_collect::util::sparse_matrix_format_type;

    SECTION("format") {
        Eigen::SparseMatrix<double> mat(2, 3);  // NOLINT
        std::vector<Eigen::Triplet<double>> data;
        data.emplace_back(0, 0, 1.234);   // NOLINT
        data.emplace_back(0, 2, 2.345);   // NOLINT
        data.emplace_back(1, 1, -3.456);  // NOLINT
        mat.setFromTriplets(data.begin(), data.end());

        SECTION("one line") {
            ApprovalTests::Approvals::verify(fmt::format("{:> 6.3f}",
                format_sparse_matrix(
                    mat, sparse_matrix_format_type::one_line)));
        }

        SECTION("multiple line") {
            ApprovalTests::Approvals::verify(fmt::format("{:> 6.3f}",
                format_sparse_matrix(
                    mat, sparse_matrix_format_type::multi_line)));
        }
    }
}