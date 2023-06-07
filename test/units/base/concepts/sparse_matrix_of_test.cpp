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
 * \brief Test of sparse_matrix_of class.
 */
#include "num_collect/base/concepts/sparse_matrix_of.h"

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::concepts::sparse_matrix_of") {
    using num_collect::concepts::sparse_matrix_of;

    SECTION("check") {
        STATIC_REQUIRE_FALSE(sparse_matrix_of<int, double>);
        STATIC_REQUIRE_FALSE(sparse_matrix_of<Eigen::ArrayXd, double>);
        STATIC_REQUIRE_FALSE(sparse_matrix_of<Eigen::MatrixXd, double>);
        STATIC_REQUIRE(sparse_matrix_of<Eigen::SparseMatrix<double>, double>);
        STATIC_REQUIRE_FALSE(
            sparse_matrix_of<Eigen::SparseMatrix<float>, double>);
    }
}
