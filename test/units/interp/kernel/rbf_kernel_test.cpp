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
 * \brief Test of rbf_kernel class.
 */
#include "num_collect/interp/kernel/rbf_kernel.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"

TEST_CASE("num_collect::interp::kernel::rbf_kernel") {
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::rbf_kernel;

    SECTION("calculate kernel") {
        const auto kernel = rbf_kernel<euclidean_distance<Eigen::Vector3d>,
            gaussian_rbf<double>>();

        const auto var1 = Eigen::Vector3d(1.234, 2.345, 3.456);
        const auto var2 = Eigen::Vector3d(1.357, 2.468, 3.579);
        const double expected = std::exp(-(var1 - var2).squaredNorm());
        REQUIRE_THAT(kernel(var1, var2), Catch::Matchers::WithinRel(expected));
    }

    SECTION("set length parameter") {
        auto kernel = rbf_kernel<euclidean_distance<Eigen::Vector3d>,
            gaussian_rbf<double>>();

        constexpr double len_param = 0.1;
        kernel.len_param(len_param);
        REQUIRE_THAT(kernel.len_param(), Catch::Matchers::WithinRel(len_param));

        const auto var1 = Eigen::Vector3d(1.234, 2.345, 3.456);
        const auto var2 = Eigen::Vector3d(1.357, 2.468, 3.579);
        const double expected =
            std::exp(-(var1 - var2).squaredNorm() / (len_param * len_param));
        REQUIRE_THAT(kernel(var1, var2), Catch::Matchers::WithinRel(expected));
    }

    SECTION("set length parameter via kernel parameter") {
        auto kernel = rbf_kernel<euclidean_distance<Eigen::Vector3d>,
            gaussian_rbf<double>>();

        constexpr double len_param = 0.1;
        kernel.kernel_param(std::log10(len_param));
        REQUIRE_THAT(kernel.kernel_param(),
            Catch::Matchers::WithinRel(std::log10(len_param)));

        const auto var1 = Eigen::Vector3d(1.234, 2.345, 3.456);
        const auto var2 = Eigen::Vector3d(1.357, 2.468, 3.579);
        const double expected =
            std::exp(-(var1 - var2).squaredNorm() / (len_param * len_param));
        REQUIRE_THAT(kernel(var1, var2), Catch::Matchers::WithinRel(expected));
    }

    SECTION("call kernel_param_search_region") {
        const auto kernel = rbf_kernel<euclidean_distance<Eigen::Vector3d>,
            gaussian_rbf<double>>();

        const auto list = std::vector<Eigen::Vector3d,
            Eigen::aligned_allocator<Eigen::Vector3d>>{
            Eigen::Vector3d(1.234, 2.345, 3.456),
            Eigen::Vector3d(1.357, 2.468, 3.579)};
        const auto [lower, upper] = kernel.kernel_param_search_region(list);
        REQUIRE(lower < upper);
    }

    SECTION("call kernel_param_search_region with only one variable") {
        const auto kernel = rbf_kernel<euclidean_distance<Eigen::Vector3d>,
            gaussian_rbf<double>>();

        const auto list = std::vector<Eigen::Vector3d,
            Eigen::aligned_allocator<Eigen::Vector3d>>{
            Eigen::Vector3d(1.234, 2.345, 3.456)};
        REQUIRE_THROWS((void)kernel.kernel_param_search_region(list));
    }

    SECTION("call kernel_param_search_region without variable") {
        const auto kernel = rbf_kernel<euclidean_distance<Eigen::Vector3d>,
            gaussian_rbf<double>>();

        const auto list = std::vector<Eigen::Vector3d,
            Eigen::aligned_allocator<Eigen::Vector3d>>{};
        REQUIRE_THROWS((void)kernel.kernel_param_search_region(list));
    }
}
