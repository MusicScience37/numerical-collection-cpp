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
 * \brief Test of is_eigen_vector class.
 */
#include "num_collect/util/is_eigen_vector.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::is_eigen_vector") {
    using num_collect::is_eigen_vector;

    SECTION("check") {
        STATIC_REQUIRE_FALSE(is_eigen_vector<int>::value);
        STATIC_REQUIRE_FALSE(is_eigen_vector<Eigen::ArrayXd>::value);
        STATIC_REQUIRE_FALSE(is_eigen_vector<Eigen::MatrixXd>::value);
        STATIC_REQUIRE(is_eigen_vector<Eigen::VectorXd>::value);
        STATIC_REQUIRE(is_eigen_vector<Eigen::Vector3f>::value);
    }
}

TEST_CASE("num_collect::is_eigen_vector_v") {
    using num_collect::is_eigen_vector_v;

    SECTION("check") {
        STATIC_REQUIRE_FALSE(is_eigen_vector_v<int>);
        STATIC_REQUIRE_FALSE(is_eigen_vector_v<Eigen::ArrayXd>);
        STATIC_REQUIRE_FALSE(is_eigen_vector_v<Eigen::MatrixXd>);
        STATIC_REQUIRE(is_eigen_vector_v<Eigen::VectorXd>);
        STATIC_REQUIRE(is_eigen_vector_v<Eigen::Vector3f>);
    }
}
