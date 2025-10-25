/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of nearest_neighbor_searcher class.
 */
#include "num_collect/util/nearest_neighbor_searcher.h"

#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/index_type.h"
#include "num_collect/util/vector.h"

TEST_CASE("num_collect::util::nearest_neighbor_searcher") {
    using num_collect::index_type;
    using num_collect::util::nearest_neighbor_searcher;
    using num_collect::util::vector;

    SECTION("1D points") {
        const auto points = vector{1.0, 2.0, 4.0, 5.0};

        const nearest_neighbor_searcher<double> searcher(points);

        SECTION("k nearest neighbors") {
            const double query_point = 2.5;
            constexpr index_type num_neighbors = 2;
            vector<std::pair<index_type, double>> indices_and_distances;

            searcher.find_k_nearest_neighbors(
                num_neighbors, query_point, indices_and_distances);

            REQUIRE(indices_and_distances.size() == 2);
            CHECK(indices_and_distances[0].first == 1);
            CHECK_THAT(indices_and_distances[0].second,
                Catch::Matchers::WithinRel(0.5));
            CHECK(indices_and_distances[1].first == 2);
            CHECK_THAT(indices_and_distances[1].second,
                Catch::Matchers::WithinRel(1.5));
        }

        SECTION("neighbors within radius") {
            const double query_point = 2.5;
            constexpr double radius = 0.6;
            vector<std::pair<index_type, double>> indices_and_distances;

            searcher.find_neighbors_within_radius(
                radius, query_point, indices_and_distances);

            REQUIRE(indices_and_distances.size() == 1);
            CHECK(indices_and_distances[0].first == 1);
            CHECK_THAT(indices_and_distances[0].second,
                Catch::Matchers::WithinRel(0.5));
        }
    }

    SECTION("2D points") {
        const auto points =
            vector{Eigen::Vector2d(1.0, 2.0), Eigen::Vector2d(2.0, 6.0),
                Eigen::Vector2d(4.0, 5.0), Eigen::Vector2d(5.0, 1.0)};

        const nearest_neighbor_searcher<Eigen::Vector2d> searcher(points);

        SECTION("k nearest neighbors") {
            const Eigen::Vector2d query_point(2.0, 3.0);
            constexpr index_type num_neighbors = 2;
            vector<std::pair<index_type, double>> indices_and_distances;

            searcher.find_k_nearest_neighbors(
                num_neighbors, query_point, indices_and_distances);

            REQUIRE(indices_and_distances.size() == 2);
            CHECK(indices_and_distances[0].first == 0);
            CHECK_THAT(indices_and_distances[0].second,
                Catch::Matchers::WithinRel(std::sqrt(2.0)));
            CHECK(indices_and_distances[1].first == 2);
            CHECK_THAT(indices_and_distances[1].second,
                Catch::Matchers::WithinRel(std::sqrt(8.0)));
        }

        SECTION("neighbors within radius") {
            const Eigen::Vector2d query_point(2.0, 3.0);
            constexpr double radius = 3.1;
            vector<std::pair<index_type, double>> indices_and_distances;

            searcher.find_neighbors_within_radius(
                radius, query_point, indices_and_distances);

            REQUIRE(indices_and_distances.size() == 3);
            CHECK(indices_and_distances[0].first == 0);
            CHECK_THAT(indices_and_distances[0].second,
                Catch::Matchers::WithinRel(std::sqrt(2.0)));
            CHECK(indices_and_distances[1].first == 2);
            CHECK_THAT(indices_and_distances[1].second,
                Catch::Matchers::WithinRel(std::sqrt(8.0)));
            CHECK(indices_and_distances[2].first == 1);
            CHECK_THAT(indices_and_distances[2].second,
                Catch::Matchers::WithinRel(3.0));
        }
    }
}
