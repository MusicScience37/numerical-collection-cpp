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
 * \brief Test of norm function.
 */
#include "num_collect/base/norm.h"

#include <cmath>
#include <complex>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::norm") {
    SECTION("norm of Vector2d") {
        const auto var = Eigen::Vector2d(2.0, 3.0);
        const auto norm = num_collect::norm(var);
        const auto true_norm = var.norm();
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }

    SECTION("norm of VectorXd") {
        const Eigen::VectorXd var =
            (Eigen::VectorXd(5) << 1.0, -2.0, 3.0, 4.0, -5.0).finished();
        const auto norm = num_collect::norm(var);
        const auto true_norm = var.norm();
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }

    SECTION("norm of Matrix2d") {
        const Eigen::Matrix2d var =
            (Eigen::Matrix2d() << 1.0, -2.0, 3.0, 4.0).finished();
        const auto norm = num_collect::norm(var);
        const auto true_norm = var.norm();
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }

    SECTION("norm of Matrix2d") {
        const Eigen::MatrixXd var =
            (Eigen::MatrixXd(2, 2) << 1.0, -2.0, 3.0, 4.0).finished();
        const auto norm = num_collect::norm(var);
        const auto true_norm = var.norm();
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }

    SECTION("norm of float") {
        constexpr float var = -2.345F;
        const float norm = num_collect::norm(var);
        const float true_norm = std::abs(var);
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }

    SECTION("norm of double") {
        constexpr double var = -2.345;
        const double norm = num_collect::norm(var);
        const double true_norm = std::abs(var);
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }

    SECTION("norm of long double") {
        constexpr long double var = -2.345;
        const auto norm = static_cast<double>(num_collect::norm(var));
        const auto true_norm = static_cast<double>(std::abs(var));
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }

    SECTION("norm of complex") {
        const auto var = std::complex<double>(1.0, -2.0);
        const double norm = num_collect::norm(var);
        const double true_norm = std::abs(var);
        REQUIRE_THAT(norm, Catch::Matchers::WithinRel(true_norm));
    }
}
