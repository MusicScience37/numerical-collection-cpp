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
 * \brief Test of compute_polynomial_term_matrix function.
 */
#include "num_collect/rbf/compute_polynomial_term_matrix.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "eigen_approx.h"

TEST_CASE("num_collect::rbf::compute_polynomial_term_matrix") {
    using num_collect::rbf::compute_polynomial_term_matrix;

    SECTION("for 1 dimension") {
        SECTION("calculate constant matrix") {
            const auto variables = std::vector<double>{0.0, 0.1, 0.2, 0.3};

            Eigen::MatrixXd matrix;
            compute_polynomial_term_matrix<0>(variables, matrix);

            const Eigen::MatrixXd expected_matrix{{1.0}, {1.0}, {1.0}, {1.0}};
            CHECK_THAT(matrix, eigen_approx(expected_matrix));
        }

        SECTION("calculate polynomial term with 1 degree") {
            const auto variables = std::vector<double>{0.0, 0.1, 0.2, 0.3};

            Eigen::MatrixXd matrix;
            compute_polynomial_term_matrix<1>(variables, matrix);

            const Eigen::MatrixXd expected_matrix{
                {1.0, 0.0}, {1.0, 0.1}, {1.0, 0.2}, {1.0, 0.3}};
            CHECK_THAT(matrix, eigen_approx(expected_matrix));
        }

        SECTION("calculate polynomial term with 2 degree") {
            const auto variables = std::vector<double>{0.0, 0.1, 0.2, 0.3};

            Eigen::MatrixXd matrix;
            compute_polynomial_term_matrix<2>(variables, matrix);

            const Eigen::MatrixXd expected_matrix{{1.0, 0.0, 0.0},
                {1.0, 0.1, 0.01}, {1.0, 0.2, 0.04}, {1.0, 0.3, 0.09}};
            CHECK_THAT(matrix, eigen_approx(expected_matrix));
        }
    }

    SECTION("for 2 dimensions") {
        const auto variables = std::vector{
            Eigen::Vector2d{0.0, 0.1},
            Eigen::Vector2d{0.2, 0.3},
            Eigen::Vector2d{0.4, 0.5},
            Eigen::Vector2d{0.6, 0.7},
            Eigen::Vector2d{0.8, 0.9},
        };

        SECTION("calculate constant matrix") {
            Eigen::MatrixXd matrix;
            compute_polynomial_term_matrix<0>(variables, matrix);

            const Eigen::MatrixXd expected_matrix{
                {1.0}, {1.0}, {1.0}, {1.0}, {1.0}};
            CHECK_THAT(matrix, eigen_approx(expected_matrix));
        }

        SECTION("calculate polynomial term with 1 degree") {
            Eigen::MatrixXd matrix;
            compute_polynomial_term_matrix<1>(variables, matrix);

            const Eigen::MatrixXd expected_matrix{{1.0, 0.0, 0.1},
                {1.0, 0.2, 0.3}, {1.0, 0.4, 0.5}, {1.0, 0.6, 0.7},
                {1.0, 0.8, 0.9}};
            CHECK_THAT(matrix, eigen_approx(expected_matrix));
        }
    }
}
