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
 * \brief Test of compute_strong_connection_list function.
 */
#include "num_collect/linear/impl/amg/compute_strong_connection_list.h"

#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "num_collect/linear/impl/amg/node_connection_list.h"

TEST_CASE("num_collect::linear::impl::amg::compute_strong_connection_list") {
    using num_collect::linear::impl::amg::compute_strong_connection_list;
    using num_collect::linear::impl::amg::node_connection_list;

    SECTION("create a list") {
        constexpr double strong_coeff_rate_threshold = 0.5;
        Eigen::SparseMatrix<double, Eigen::RowMajor> matrix(3, 3);
        std::vector<Eigen::Triplet<double>> coefficients;
        coefficients.emplace_back(0, 0, 1.0);    // NOLINT
        coefficients.emplace_back(0, 1, -0.5);   // NOLINT
        coefficients.emplace_back(0, 2, -0.25);  // NOLINT
        coefficients.emplace_back(1, 0, -0.5);   // NOLINT
        coefficients.emplace_back(1, 1, 1.0);    // NOLINT
        coefficients.emplace_back(1, 2, -0.24);  // NOLINT
        coefficients.emplace_back(2, 1, -0.5);   // NOLINT
        coefficients.emplace_back(2, 2, 1.0);    // NOLINT
        matrix.setFromTriplets(coefficients.begin(), coefficients.end());

        const auto list =
            compute_strong_connection_list(matrix, strong_coeff_rate_threshold);

        CHECK(list.num_nodes() == 3);
        CHECK_THAT(list.connected_nodes_to(0),
            Catch::Matchers::RangeEquals(std::vector<int>{1, 2}));
        CHECK_THAT(list.connected_nodes_to(1),
            Catch::Matchers::RangeEquals(std::vector<int>{0}));
        CHECK_THAT(list.connected_nodes_to(2),
            Catch::Matchers::RangeEquals(std::vector<int>{1}));
    }
}
