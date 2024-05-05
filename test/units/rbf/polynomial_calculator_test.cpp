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
 * \brief Test of polynomial_calculator class.
 */
#include "num_collect/rbf/polynomial_calculator.h"

#include <vector>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"

TEST_CASE("num_collect::rbf::polynomial_calculator") {
    using num_collect::rbf::polynomial_calculator;

    SECTION("for scalar variables") {
        SECTION("for degree 0") {
            polynomial_calculator<double, 0> calculator;

            SECTION("calculate matrix of polynomial terms") {
                const auto variables = std::vector<double>{0.0, 0.1, 0.2, 0.3};

                Eigen::MatrixXd matrix;
                calculator.compute_polynomial_term_matrix(variables, matrix);

                const Eigen::MatrixXd expected_matrix{
                    {1.0}, {1.0}, {1.0}, {1.0}};
                CHECK_THAT(matrix, eigen_approx(expected_matrix));
            }

            SECTION("evaluate a polynomial with a variable") {
                const Eigen::VectorXd coefficients{{0.2}};
                constexpr double variable = 0.7;
                constexpr double expected_value = 0.2;

                CHECK_THAT(calculator.evaluate_polynomial_for_variable(
                               variable, coefficients),
                    Catch::Matchers::WithinRel(expected_value));
            }
        }

        SECTION("for degree 1") {
            polynomial_calculator<double, 1> calculator;

            SECTION("calculate matrix of polynomial terms") {
                const auto variables = std::vector<double>{0.0, 0.1, 0.2, 0.3};

                Eigen::MatrixXd matrix;
                calculator.compute_polynomial_term_matrix(variables, matrix);

                const Eigen::MatrixXd expected_matrix{
                    {1.0, 0.0}, {1.0, 0.1}, {1.0, 0.2}, {1.0, 0.3}};
                CHECK_THAT(matrix, eigen_approx(expected_matrix));
            }

            SECTION("evaluate a polynomial with a variable") {
                const Eigen::VectorXd coefficients{{0.2, 0.4}};
                constexpr double variable = 0.7;
                constexpr double expected_value = 0.48;

                CHECK_THAT(calculator.evaluate_polynomial_for_variable(
                               variable, coefficients),
                    Catch::Matchers::WithinRel(expected_value));
            }
        }

        SECTION("for degree 2") {
            polynomial_calculator<double, 2> calculator;

            SECTION("calculate matrix of polynomial terms") {
                const auto variables = std::vector<double>{0.0, 0.1, 0.2, 0.3};

                Eigen::MatrixXd matrix;
                calculator.compute_polynomial_term_matrix(variables, matrix);

                const Eigen::MatrixXd expected_matrix{{1.0, 0.0, 0.0},
                    {1.0, 0.1, 0.01}, {1.0, 0.2, 0.04}, {1.0, 0.3, 0.09}};
                CHECK_THAT(matrix, eigen_approx(expected_matrix));
            }

            SECTION("evaluate a polynomial with a variable") {
                const Eigen::VectorXd coefficients{{0.2, 0.4, 0.6}};
                constexpr double variable = 0.7;
                constexpr double expected_value = 0.774;

                CHECK_THAT(calculator.evaluate_polynomial_for_variable(
                               variable, coefficients),
                    Catch::Matchers::WithinRel(expected_value));
            }
        }
    }
}
