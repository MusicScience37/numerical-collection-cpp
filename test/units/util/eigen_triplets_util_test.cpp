/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of utility functions for Eigen::Triplet.
 */
#include "num_collect/util/eigen_triplets_util.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/util/vector.h"

TEST_CASE("num_collect::util::eigen_triplets::shift_rows") {
    using num_collect::util::eigen_triplets::shift_rows;

    SECTION("shift rows") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);

        num_collect::util::vector<Eigen::Triplet<double>> shifted_triplets;
        shifted_triplets.append_range(triplets | shift_rows(2));

        REQUIRE(shifted_triplets.size() == 3);
        CHECK(shifted_triplets[0].row() == 2);
        CHECK(shifted_triplets[0].col() == 0);
        CHECK(shifted_triplets[0].value() == 1.0);
        CHECK(shifted_triplets[1].row() == 3);
        CHECK(shifted_triplets[1].col() == 0);
        CHECK(shifted_triplets[1].value() == 2.0);
        CHECK(shifted_triplets[2].row() == 3);
        CHECK(shifted_triplets[2].col() == 1);
        CHECK(shifted_triplets[2].value() == 3.0);
    }
}

TEST_CASE("num_collect::util::eigen_triplets::shift_columns") {
    using num_collect::util::eigen_triplets::shift_columns;

    SECTION("shift columns") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);

        num_collect::util::vector<Eigen::Triplet<double>> shifted_triplets;
        shifted_triplets.append_range(triplets | shift_columns(3));

        REQUIRE(shifted_triplets.size() == 3);
        CHECK(shifted_triplets[0].row() == 0);
        CHECK(shifted_triplets[0].col() == 3);
        CHECK(shifted_triplets[0].value() == 1.0);
        CHECK(shifted_triplets[1].row() == 1);
        CHECK(shifted_triplets[1].col() == 3);
        CHECK(shifted_triplets[1].value() == 2.0);
        CHECK(shifted_triplets[2].row() == 1);
        CHECK(shifted_triplets[2].col() == 4);
        CHECK(shifted_triplets[2].value() == 3.0);
    }
}

TEST_CASE("num_collect::util::eigen_triplets::shift_rows_and_columns") {
    using num_collect::util::eigen_triplets::shift_rows_and_columns;

    SECTION("shift rows and columns") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);

        num_collect::util::vector<Eigen::Triplet<double>> shifted_triplets;
        shifted_triplets.append_range(triplets | shift_rows_and_columns(2, 3));

        REQUIRE(shifted_triplets.size() == 3);
        CHECK(shifted_triplets[0].row() == 2);
        CHECK(shifted_triplets[0].col() == 3);
        CHECK(shifted_triplets[0].value() == 1.0);
        CHECK(shifted_triplets[1].row() == 3);
        CHECK(shifted_triplets[1].col() == 3);
        CHECK(shifted_triplets[1].value() == 2.0);
        CHECK(shifted_triplets[2].row() == 3);
        CHECK(shifted_triplets[2].col() == 4);
        CHECK(shifted_triplets[2].value() == 3.0);
    }
}

TEST_CASE("num_collect::util::eigen_triplets::filter_rows") {
    using num_collect::util::eigen_triplets::filter_rows;

    SECTION("filter rows") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);
        triplets.emplace_back(2, 0, 4.0);
        triplets.emplace_back(4, 0, 5.0);
        triplets.emplace_back(5, 0, 6.0);
        triplets.emplace_back(6, 0, 7.0);

        num_collect::util::vector<Eigen::Triplet<double>> filtered_triplets;
        filtered_triplets.append_range(triplets | filter_rows(1, 5));

        REQUIRE(filtered_triplets.size() == 4);
        CHECK(filtered_triplets[0].row() == 1);
        CHECK(filtered_triplets[0].col() == 0);
        CHECK(filtered_triplets[0].value() == 2.0);
        CHECK(filtered_triplets[1].row() == 1);
        CHECK(filtered_triplets[1].col() == 1);
        CHECK(filtered_triplets[1].value() == 3.0);
        CHECK(filtered_triplets[2].row() == 2);
        CHECK(filtered_triplets[2].col() == 0);
        CHECK(filtered_triplets[2].value() == 4.0);
        CHECK(filtered_triplets[3].row() == 4);
        CHECK(filtered_triplets[3].col() == 0);
        CHECK(filtered_triplets[3].value() == 5.0);
    }
}

TEST_CASE("num_collect::util::eigen_triplets::filter_columns") {
    using num_collect::util::eigen_triplets::filter_columns;

    SECTION("filter columns") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 1, 2.0);
        triplets.emplace_back(0, 2, 3.0);
        triplets.emplace_back(1, 4, 4.0);
        triplets.emplace_back(2, 5, 5.0);
        triplets.emplace_back(3, 6, 6.0);

        num_collect::util::vector<Eigen::Triplet<double>> filtered_triplets;
        filtered_triplets.append_range(triplets | filter_columns(1, 5));

        REQUIRE(filtered_triplets.size() == 3);
        CHECK(filtered_triplets[0].row() == 1);
        CHECK(filtered_triplets[0].col() == 1);
        CHECK(filtered_triplets[0].value() == 2.0);
        CHECK(filtered_triplets[1].row() == 0);
        CHECK(filtered_triplets[1].col() == 2);
        CHECK(filtered_triplets[1].value() == 3.0);
        CHECK(filtered_triplets[2].row() == 1);
        CHECK(filtered_triplets[2].col() == 4);
        CHECK(filtered_triplets[2].value() == 4.0);
    }
}

TEST_CASE("num_collect::util::eigen_triplets::to_vector") {
    using num_collect::util::eigen_triplets::to_vector;

    SECTION("convert a range to a vector") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);

        const auto result = to_vector(triplets);

        REQUIRE(result.size() == 3);
        CHECK(result[0].row() == 0);
        CHECK(result[0].col() == 0);
        CHECK(result[0].value() == 1.0);
        CHECK(result[1].row() == 1);
        CHECK(result[1].col() == 0);
        CHECK(result[1].value() == 2.0);
        CHECK(result[2].row() == 1);
        CHECK(result[2].col() == 1);
        CHECK(result[2].value() == 3.0);
    }

    SECTION("convert a range to a vector using range adaptor closure") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);

        const auto result = triplets | to_vector();

        STATIC_REQUIRE(std::same_as<std::decay_t<decltype(result)>,
            num_collect::util::vector<Eigen::Triplet<double>>>);
        REQUIRE(result.size() == 3);
        CHECK(result[0].row() == 0);
        CHECK(result[0].col() == 0);
        CHECK(result[0].value() == 1.0);
        CHECK(result[1].row() == 1);
        CHECK(result[1].col() == 0);
        CHECK(result[1].value() == 2.0);
        CHECK(result[2].row() == 1);
        CHECK(result[2].col() == 1);
        CHECK(result[2].value() == 3.0);
    }
}

TEST_CASE("num_collect::util::eigen_triplets::to_sparse_matrix") {
    using num_collect::util::eigen_triplets::to_sparse_matrix;

    SECTION("convert a range to a sparse matrix") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);

        const auto result =
            to_sparse_matrix<Eigen::SparseMatrix<double>>(triplets, 2, 2);

        STATIC_REQUIRE(std::same_as<std::decay_t<decltype(result)>,
            Eigen::SparseMatrix<double>>);
        REQUIRE(result.rows() == 2);
        REQUIRE(result.cols() == 2);
        CHECK(result.coeff(0, 0) == 1.0);
        CHECK(result.coeff(0, 1) == 0.0);
        CHECK(result.coeff(1, 0) == 2.0);
        CHECK(result.coeff(1, 1) == 3.0);
    }

    SECTION("convert a range to a sparse matrix using range adaptor closure") {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(1, 0, 2.0);
        triplets.emplace_back(1, 1, 3.0);

        const auto result = triplets |
            to_sparse_matrix<Eigen::SparseMatrix<double, Eigen::RowMajor>>(
                2, 2);

        STATIC_REQUIRE(std::same_as<std::decay_t<decltype(result)>,
            Eigen::SparseMatrix<double, Eigen::RowMajor>>);
        REQUIRE(result.rows() == 2);
        REQUIRE(result.cols() == 2);
        CHECK(result.coeff(0, 0) == 1.0);
        CHECK(result.coeff(0, 1) == 0.0);
        CHECK(result.coeff(1, 0) == 2.0);
        CHECK(result.coeff(1, 1) == 3.0);
    }
}
