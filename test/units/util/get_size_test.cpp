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
 * \brief Test of get_size function.
 */
#include "num_collect/util/get_size.h"

#include <cmath>
#include <complex>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::get_size") {
    SECTION("float") {
        const float data = 0.0F;
        REQUIRE(num_collect::get_size(data) == 1);
    }

    SECTION("double") {
        const double data = 0.0;
        REQUIRE(num_collect::get_size(data) == 1);
    }

    SECTION("complex<double>") {
        const std::complex<double> data = 0.0;
        REQUIRE(num_collect::get_size(data) == 1);
    }

    SECTION("Eigen::MatrixXd") {
        const num_collect::index_type rows = 5;
        const num_collect::index_type cols = 7;
        const Eigen::MatrixXd mat = Eigen::MatrixXd::Zero(rows, cols);
        REQUIRE(num_collect::get_size(mat) == rows * cols);
    }

    SECTION("Eigen::VectorXd") {
        const num_collect::index_type size = 7;
        const Eigen::VectorXd mat = Eigen::VectorXd::Zero(size);
        REQUIRE(num_collect::get_size(mat) == size);
    }
}
