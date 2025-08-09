/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of real_scalar_sparse_matrix concept.
 */
#include "num_collect/base/concepts/real_scalar_sparse_matrix.h"

#include <complex>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::concepts::real_scalar_sparse_matrix") {
    using num_collect::concepts::real_scalar_sparse_matrix;

    SECTION("check some types") {
        STATIC_REQUIRE(real_scalar_sparse_matrix<Eigen::SparseMatrix<double>>);
        STATIC_REQUIRE(real_scalar_sparse_matrix<Eigen::SparseMatrix<float>>);
        STATIC_REQUIRE_FALSE(
            real_scalar_sparse_matrix<Eigen::SparseMatrix<int>>);
        STATIC_REQUIRE_FALSE(real_scalar_sparse_matrix<
            Eigen::SparseMatrix<std::complex<double>>>);
        STATIC_REQUIRE_FALSE(real_scalar_sparse_matrix<Eigen::MatrixXd>);
    }
}
