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
 * \brief Test of rbf_fd_polynomial_assembler class.
 */
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"

#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"

TEST_CASE("num_collect::rbf::rbf_fd_polynomial_assembler") {
    using num_collect::rbf::generate_halton_nodes;
    using num_collect::rbf::rbf_fd_polynomial_assembler;
    using num_collect::rbf::operators::laplacian_operator;
    using num_collect::util::nearest_neighbor_searcher;

    using scalar_type = double;
    constexpr num_collect::index_type dimensions = 2;
    constexpr num_collect::index_type num_rows = 10;
    constexpr num_collect::index_type num_columns = 50;
    using variable_type = Eigen::Vector<scalar_type, dimensions>;
    using operator_type = laplacian_operator<variable_type>;
    using assembler_type = rbf_fd_polynomial_assembler<variable_type>;

    assembler_type assembler;

    SECTION("compute a matrix") {
        const auto row_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_rows);
        const auto column_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_columns);
        const nearest_neighbor_searcher<variable_type>
            column_variables_nearest_neighbor_searcher(column_variables);
        num_collect::util::vector<Eigen::Triplet<scalar_type>> triplets;

        assembler.compute_rows<operator_type>(row_variables, column_variables,
            column_variables_nearest_neighbor_searcher, triplets, 0, 0);

        CHECK(triplets.size() == num_rows * assembler.num_neighbors());
        Eigen::SparseMatrix<scalar_type> matrix(num_rows, num_columns);
        matrix.setFromTriplets(triplets.begin(), triplets.end());

        const auto test_function = [](const variable_type& variable) {
            return variable.array().sin().prod();
        };
        const auto test_function_laplacian = [](const variable_type& variable) {
            return -2.0 * variable.array().sin().prod();
        };
        Eigen::VectorXd function_values(num_columns);
        for (num_collect::index_type i = 0; i < num_columns; ++i) {
            function_values(i) = test_function(column_variables[i]);
        }
        Eigen::VectorXd expected_laplacian(num_rows);
        for (num_collect::index_type i = 0; i < num_rows; ++i) {
            expected_laplacian(i) = test_function_laplacian(row_variables[i]);
        }
        const Eigen::VectorXd approximated_laplacian = matrix * function_values;

        comparison_approvals::verify_with_reference(
            approximated_laplacian, expected_laplacian);

        SECTION("take offsets into account") {
            const num_collect::index_type row_offset = 5;
            const num_collect::index_type column_offset = 10;
            num_collect::util::vector<Eigen::Triplet<scalar_type>>
                triplets_with_offset;

            assembler.compute_rows<operator_type>(row_variables,
                column_variables, column_variables_nearest_neighbor_searcher,
                triplets_with_offset, row_offset, column_offset);

            REQUIRE(triplets_with_offset.size() == triplets.size());
            for (num_collect::index_type i = 0; i < triplets_with_offset.size();
                ++i) {
                CHECK(triplets_with_offset[i].row() ==
                    row_offset + triplets[i].row());
                CHECK(triplets_with_offset[i].col() ==
                    column_offset + triplets[i].col());
                CHECK_THAT(triplets_with_offset[i].value(),
                    Catch::Matchers::WithinRel(triplets[i].value()));
            }
        }
    }

    SECTION("compute a matrix") {
        constexpr num_collect::index_type num_rows = 100;
        constexpr num_collect::index_type num_columns = 100;
        const auto row_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_rows);
        const auto column_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_columns);
        const nearest_neighbor_searcher<variable_type>
            column_variables_nearest_neighbor_searcher(column_variables);
        num_collect::util::vector<Eigen::Triplet<scalar_type>> triplets;

        assembler.compute_rows<operator_type>(row_variables, column_variables,
            column_variables_nearest_neighbor_searcher, triplets, 0, 0);

        CHECK(triplets.size() == num_rows * assembler.num_neighbors());
        Eigen::SparseMatrix<scalar_type> matrix(num_rows, num_columns);
        matrix.setFromTriplets(triplets.begin(), triplets.end());

        const auto test_function = [](const variable_type& variable) {
            return variable.array().sin().prod();
        };
        const auto test_function_laplacian = [](const variable_type& variable) {
            return -2.0 * variable.array().sin().prod();
        };
        Eigen::VectorXd function_values(num_columns);
        for (num_collect::index_type i = 0; i < num_columns; ++i) {
            function_values(i) = test_function(column_variables[i]);
        }
        Eigen::VectorXd expected_laplacian(num_rows);
        for (num_collect::index_type i = 0; i < num_rows; ++i) {
            expected_laplacian(i) = test_function_laplacian(row_variables[i]);
        }
        const Eigen::VectorXd approximated_laplacian = matrix * function_values;

        const double error_rate =
            (approximated_laplacian - expected_laplacian).norm() /
            expected_laplacian.norm();
        CHECK(error_rate < 0.1);
    }

    SECTION("check number of neighbors") {
        const auto row_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_rows);
        const auto column_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_columns);
        const nearest_neighbor_searcher<variable_type>
            column_variables_nearest_neighbor_searcher(column_variables);
        num_collect::util::vector<Eigen::Triplet<scalar_type>> triplets;

        CHECK_THROWS(assembler.num_neighbors(-1));
        CHECK_THROWS(assembler.num_neighbors(0));
        CHECK_THROWS(assembler.num_neighbors(1));
        CHECK_THROWS(assembler.num_neighbors(6));
        CHECK_NOTHROW(assembler.num_neighbors(7));

        CHECK_NOTHROW(assembler.num_neighbors(num_columns));
        CHECK_NOTHROW(assembler.compute_rows<operator_type>(row_variables,
            column_variables, column_variables_nearest_neighbor_searcher,
            triplets, 0, 0));

        CHECK_NOTHROW(assembler.num_neighbors(num_columns + 1));
        CHECK_THROWS(assembler.compute_rows<operator_type>(row_variables,
            column_variables, column_variables_nearest_neighbor_searcher,
            triplets, 0, 0));
    }
}
