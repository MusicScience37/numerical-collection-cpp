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
 * \brief Test of euclidean_distance_function class.
 */
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/distance_function.h"

TEST_CASE("num_collect::rbf::distance_functions::euclidean_distance_function") {
    using num_collect::rbf::concepts::distance_function;
    using num_collect::rbf::distance_functions::euclidean_distance_function;

    SECTION("check of concepts") {
        STATIC_REQUIRE(distance_function<euclidean_distance_function<double>>);
        STATIC_REQUIRE(
            distance_function<euclidean_distance_function<Eigen::Vector2f>>);
        STATIC_REQUIRE(
            distance_function<euclidean_distance_function<Eigen::VectorXd>>);
    }

    SECTION("calculate a distance of scalars") {
        constexpr double var1 = 1.4;
        constexpr double var2 = 2.1;
        constexpr double expected_distance = 0.7;
        const euclidean_distance_function<double> distance_function;

        const double distance = distance_function(var1, var2);

        CHECK_THAT(distance, Catch::Matchers::WithinRel(expected_distance));
    }

    SECTION("calculate a distance of vectors") {
        const auto var1 = Eigen::Vector2d{{-0.1, 1.2}};
        const auto var2 = Eigen::Vector2d{{0.2, 0.8}};
        constexpr double expected_distance = 0.5;
        const euclidean_distance_function<Eigen::Vector2d> distance_function;

        const double distance = distance_function(var1, var2);

        CHECK_THAT(distance, Catch::Matchers::WithinRel(expected_distance));
    }
}
