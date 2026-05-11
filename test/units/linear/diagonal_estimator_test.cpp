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
 * \brief Test of diagonal_estimator class.
 */
#include "num_collect/linear/diagonal_estimator.h"

#include <random>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::linear::diagonal_estimator") {
    using num_collect::linear::diagonal_estimator;

    SECTION("estimate diagonal elements") {
        const Eigen::Matrix3d matrix{
            {1.0, 0.1, -0.1}, {0.2, 2.0, 0.2}, {-0.3, 0.3, 3.0}};
        const auto coeff_function = [&matrix](const Eigen::Vector3d& input,
                                        Eigen::Vector3d& output) {
            output = matrix * input;
        };

        diagonal_estimator<Eigen::Vector3d> estimator;
        Eigen::Vector3d estimated_diagonal;
        estimator.estimate(coeff_function, matrix.rows(), estimated_diagonal);

        const Eigen::Vector3d true_diagonal = matrix.diagonal();
        constexpr double absolute_tolerance = 0.2;  // Worst case error.
        CHECK(estimated_diagonal.size() == true_diagonal.size());
        for (num_collect::index_type i = 0; i < true_diagonal.size(); ++i) {
            INFO("i = " << i);
            CHECK_THAT(estimated_diagonal[i],
                Catch::Matchers::WithinAbs(
                    true_diagonal[i], absolute_tolerance));
        }
    }

    SECTION("prepare before estimation") {
        const Eigen::Matrix3d matrix{
            {1.0, 0.1, -0.1}, {0.2, 2.0, 0.2}, {-0.3, 0.3, 3.0}};
        const auto coeff_function = [&matrix](const Eigen::Vector3d& input,
                                        Eigen::Vector3d& output) {
            output = matrix * input;
        };

        diagonal_estimator<Eigen::Vector3d> estimator;
        estimator.prepare(matrix.rows());

        Eigen::Vector3d estimated_diagonal;
        estimator.estimate(coeff_function, matrix.rows(), estimated_diagonal);

        const Eigen::Vector3d true_diagonal = matrix.diagonal();
        constexpr double absolute_tolerance = 0.2;  // Worst case error.
        CHECK(estimated_diagonal.size() == true_diagonal.size());
        for (num_collect::index_type i = 0; i < true_diagonal.size(); ++i) {
            INFO("i = " << i);
            CHECK_THAT(estimated_diagonal[i],
                Catch::Matchers::WithinAbs(
                    true_diagonal[i], absolute_tolerance));
        }
    }

    SECTION("prepare with different size") {
        const Eigen::Matrix3d matrix{
            {1.0, 0.1, -0.1}, {0.2, 2.0, 0.2}, {-0.3, 0.3, 3.0}};
        const auto coeff_function = [&matrix](const Eigen::VectorXd& input,
                                        Eigen::VectorXd& output) {
            output = matrix * input;
        };

        diagonal_estimator<Eigen::VectorXd> estimator;
        estimator.prepare(5);  // Different size.

        Eigen::VectorXd estimated_diagonal;
        estimator.estimate(coeff_function, matrix.rows(), estimated_diagonal);

        const Eigen::Vector3d true_diagonal = matrix.diagonal();
        constexpr double absolute_tolerance = 0.2;  // Worst case error.
        CHECK(estimated_diagonal.size() == true_diagonal.size());
        for (num_collect::index_type i = 0; i < true_diagonal.size(); ++i) {
            INFO("i = " << i);
            CHECK_THAT(estimated_diagonal[i],
                Catch::Matchers::WithinAbs(
                    true_diagonal[i], absolute_tolerance));
        }
    }

    SECTION("change seed") {
        diagonal_estimator<Eigen::VectorXd> estimator;

        std::vector<int> seeds{1, 2, 3};
        std::seed_seq seed(seeds.begin(), seeds.end());
        CHECK_NOTHROW(estimator.seed(seed));

        // Just check that no error occurs.
    }

    SECTION("set number of random vectors") {
        diagonal_estimator<Eigen::VectorXd> estimator;

        CHECK_NOTHROW(estimator.num_random_vectors(1));
        CHECK_NOTHROW(estimator.num_random_vectors(10));
        CHECK_THROWS(estimator.num_random_vectors(0));
    }
}
