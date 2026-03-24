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
 * \brief Test of rbf_fd_polynomial_row_calculator class.
 */
#include "num_collect/rbf/impl/rbf_fd_polynomial_row_calculator.h"

#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/polynomial_term_generator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"

TEST_CASE("num_collect::rbf::impl::rbf_fd_polynomial_row_calculator") {
    using num_collect::rbf::generate_halton_nodes;
    using num_collect::rbf::distance_functions::euclidean_distance_function;
    using num_collect::rbf::impl::rbf_fd_polynomial_row_calculator;
    using num_collect::rbf::length_parameter_calculators::
        global_length_parameter_calculator;
    using num_collect::rbf::operators::laplacian_operator;
    using num_collect::rbf::rbfs::gaussian_rbf;
    using num_collect::util::nearest_neighbor_searcher;

    using scalar_type = double;
    constexpr num_collect::index_type dimensions = 2;
    constexpr num_collect::index_type num_neighbors = 10;
    constexpr num_collect::index_type num_columns = 50;
    using variable_type = Eigen::Vector<scalar_type, dimensions>;
    using distance_function_type = euclidean_distance_function<variable_type>;
    using rbf_type = gaussian_rbf<scalar_type>;
    using polynomial_term_generator_type =
        num_collect::rbf::polynomial_term_generator<dimensions>;
    using length_parameter_calculator_type =
        global_length_parameter_calculator<distance_function_type>;
    using rbf_fd_polynomial_row_calculator_type =
        rbf_fd_polynomial_row_calculator<length_parameter_calculator_type>;

    const distance_function_type distance_function;
    const rbf_type rbf;
    const polynomial_term_generator_type polynomial_term_generator(2);
    rbf_fd_polynomial_row_calculator_type row_calculator;

    const auto test_function = [](const variable_type& variable) {
        return variable.array().sin().prod();
    };
    const auto test_function_laplacian = [](const variable_type& variable) {
        return -2.0 * variable.array().sin().prod();
    };

    SECTION("compute a row") {
        const variable_type row_variable{0.5, 0.5};
        const auto column_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_columns);
        const nearest_neighbor_searcher<variable_type>
            column_variables_nearest_neighbor_searcher(column_variables);
        const auto target_operator = laplacian_operator{row_variable};
        constexpr num_collect::index_type row_index = 0;
        constexpr num_collect::index_type column_offset = 0;
        num_collect::util::vector<Eigen::Triplet<scalar_type>> triplets;

        row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher,
            num_neighbors, triplets, row_index, column_offset);

        CHECK(triplets.size() == num_neighbors);
        Eigen::SparseMatrix<scalar_type> row_matrix(1, num_columns);
        row_matrix.setFromTriplets(triplets.begin(), triplets.end());
        Eigen::VectorXd function_values(num_columns);
        for (num_collect::index_type i = 0; i < num_columns; ++i) {
            function_values(i) = test_function(column_variables[i]);
        }
        const double approximated_value = (row_matrix * function_values)(0);
        const double true_value = test_function_laplacian(row_variable);
        constexpr double tolerance = 1e-2;
        CHECK_THAT(approximated_value,
            Catch::Matchers::WithinRel(true_value, tolerance));

        SECTION("take row_index and column_offset into account") {
            constexpr num_collect::index_type row_index = 2;
            constexpr num_collect::index_type column_offset = 5;
            num_collect::util::vector<Eigen::Triplet<scalar_type>>
                triplets_with_offset;

            row_calculator.compute_row(distance_function, rbf,
                polynomial_term_generator, target_operator, row_variable,
                column_variables, column_variables_nearest_neighbor_searcher,
                num_neighbors, triplets_with_offset, row_index, column_offset);

            REQUIRE(triplets_with_offset.size() == num_neighbors);
            for (num_collect::index_type i = 0; i < num_neighbors; ++i) {
                CHECK(triplets_with_offset[i].row() == row_index);
                CHECK(triplets_with_offset[i].col() ==
                    column_offset + triplets[i].col());
                CHECK_THAT(triplets_with_offset[i].value(),
                    Catch::Matchers::WithinRel(triplets[i].value()));
            }
        }
    }

    SECTION("check number of neighbors") {
        const variable_type row_variable{0.5, 0.5};
        const auto column_variables =
            generate_halton_nodes<scalar_type, dimensions>(num_columns);
        const nearest_neighbor_searcher<variable_type>
            column_variables_nearest_neighbor_searcher(column_variables);
        const auto target_operator = laplacian_operator{row_variable};
        constexpr num_collect::index_type row_index = 0;
        constexpr num_collect::index_type column_offset = 0;
        num_collect::util::vector<Eigen::Triplet<scalar_type>> triplets;

        CHECK_THROWS(row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher, -1,
            triplets, row_index, column_offset));
        CHECK_THROWS(row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher, 0,
            triplets, row_index, column_offset));
        CHECK_THROWS(row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher, 1,
            triplets, row_index, column_offset));
        CHECK_THROWS(row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher, 6,
            triplets, row_index, column_offset));
        CHECK_NOTHROW(row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher, 7,
            triplets, row_index, column_offset));
        CHECK_NOTHROW(row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher,
            num_columns, triplets, row_index, column_offset));
        CHECK_THROWS(row_calculator.compute_row(distance_function, rbf,
            polynomial_term_generator, target_operator, row_variable,
            column_variables, column_variables_nearest_neighbor_searcher,
            num_columns + 1, triplets, row_index, column_offset));
    }

    // TODO Test of ill-conditioned matrix.
}

// TODO tests which may be implemented in some places.
// - Test of other dimensions.
// - Test of other polynomial orders.
// - Test of other operators.
// - Test of other RBFs.
// - Test of whole system matrix.
