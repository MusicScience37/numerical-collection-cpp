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
 * \brief Test of calc_kernel_mat function.
 */
#include "num_collect/interp/kernel/calc_kernel_mat.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"
#include "num_collect/interp/kernel/rbf_kernel.h"

TEST_CASE("num_collect::interp::kernel::calc_kernel_mat") {
    using num_collect::interp::kernel::calc_kernel_mat;
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::rbf_kernel;

    SECTION("calculate") {
        const auto kernel = rbf_kernel<euclidean_distance<Eigen::Vector3d>,
            gaussian_rbf<double>>();
        const auto list = std::vector<Eigen::Vector3d,
            Eigen::aligned_allocator<Eigen::Vector3d>>{
            Eigen::Vector3d(1.234, 2.345, 3.456),
            Eigen::Vector3d(1.357, 2.468, 3.579)};

        const Eigen::MatrixXd mat = calc_kernel_mat(kernel, list);

        REQUIRE(mat.rows() == 2);
        REQUIRE(mat.cols() == 2);

        const double non_diagonal_elem =
            std::exp(-(list[0] - list[1]).squaredNorm());
        REQUIRE_THAT(mat(0, 0), Catch::Matchers::WithinRel(1.0));
        REQUIRE_THAT(mat(0, 1), Catch::Matchers::WithinRel(non_diagonal_elem));
        REQUIRE_THAT(mat(1, 0), Catch::Matchers::WithinRel(non_diagonal_elem));
        REQUIRE_THAT(mat(1, 1), Catch::Matchers::WithinRel(1.0));
    }
}
