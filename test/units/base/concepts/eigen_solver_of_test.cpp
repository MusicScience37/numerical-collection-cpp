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
 * \brief Test of eigen_solver_of concept.
 */
#include "num_collect/base/concepts/eigen_solver_of.h"

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SVD>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::concepts::eigen_solver_of") {
    using num_collect::concepts::eigen_solver_of;

    SECTION("check") {
        STATIC_REQUIRE(eigen_solver_of<Eigen::PartialPivLU<Eigen::MatrixXd>,
            Eigen::MatrixXd, Eigen::VectorXd>);
        STATIC_REQUIRE(eigen_solver_of<Eigen::LLT<Eigen::MatrixXd>,
            Eigen::MatrixXd, Eigen::VectorXd>);
        STATIC_REQUIRE(eigen_solver_of<Eigen::JacobiSVD<Eigen::MatrixXd>,
            Eigen::MatrixXd, Eigen::VectorXd>);
    }
}
