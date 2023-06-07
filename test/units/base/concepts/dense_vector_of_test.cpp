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
 * \brief Test of dense_vector_of class.
 */
#include "num_collect/base/concepts/dense_vector_of.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::concepts::dense_vector_of") {
    using num_collect::concepts::dense_vector_of;

    SECTION("check") {
        STATIC_REQUIRE_FALSE(dense_vector_of<int, double>);
        STATIC_REQUIRE_FALSE(dense_vector_of<Eigen::ArrayXd, double>);
        STATIC_REQUIRE_FALSE(dense_vector_of<Eigen::MatrixXd, double>);
        STATIC_REQUIRE(dense_vector_of<Eigen::VectorXd, double>);
        STATIC_REQUIRE_FALSE(dense_vector_of<Eigen::MatrixXi, double>);
        STATIC_REQUIRE_FALSE(dense_vector_of<Eigen::VectorXi, double>);
    }
}
