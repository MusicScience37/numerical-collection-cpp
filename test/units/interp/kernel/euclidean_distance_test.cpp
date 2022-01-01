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
 * \brief Test of euclidean_distance class.
 */
#include "num_collect/interp/kernel/euclidean_distance.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::interp::kernel::euclidean_distance") {
    using num_collect::interp::kernel::euclidean_distance;

    SECTION("calculate distance of double") {
        const auto dist = euclidean_distance<double>();
        constexpr double var1 = 1.234;
        constexpr double var2 = 3.1415;
        constexpr double expected = var2 - var1;
        REQUIRE_THAT(dist(var1, var2), Catch::Matchers::WithinRel(expected));
        REQUIRE_THAT(dist(var2, var1), Catch::Matchers::WithinRel(expected));
    }

    SECTION("calculate distance of vectors") {
        const auto dist = euclidean_distance<Eigen::Vector3d>();
        const auto var1 = Eigen::Vector3d(1.234, 2.345, 3.456);
        const auto var2 = Eigen::Vector3d(1.357, 2.468, 3.579);
        const double expected = (var1 - var2).norm();
        REQUIRE_THAT(dist(var1, var2), Catch::Matchers::WithinRel(expected));
        REQUIRE_THAT(dist(var2, var1), Catch::Matchers::WithinRel(expected));
    }
}
