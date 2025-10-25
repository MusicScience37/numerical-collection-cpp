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
 * \brief Benchmarks of nearest neighbor search.
 */
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/plot_options.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"

class nearest_neighbors_search_fixture : public stat_bench::FixtureBase {
public:
    nearest_neighbors_search_fixture() {
        this->add_param<num_collect::index_type>("num_points")
            ->add(100)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(1000)
            ->add(10000)
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        num_points_ = context.get_param<num_collect::index_type>("num_points");
    }

    [[nodiscard]] auto num_points() const -> num_collect::index_type {
        return num_points_;
    }

private:
    num_collect::index_type num_points_{};
};

STAT_BENCH_CASE_F(
    nearest_neighbors_search_fixture, "find_k_nearest_neighbors", "1D") {
    const auto points =
        num_collect::rbf::generate_1d_halton_nodes<double>(num_points());

    constexpr num_collect::index_type max_num_neighbors = 10;
    num_collect::util::vector<std::pair<num_collect::index_type, double>>
        indices_and_distances;

    STAT_BENCH_MEASURE() {
        const num_collect::util::nearest_neighbor_searcher<double> searcher(
            points);

        for (num_collect::index_type i = 0; i < num_points(); ++i) {
            const double& query = points[i];
            searcher.find_k_nearest_neighbors(
                max_num_neighbors, query, indices_and_distances);
        }
    };
}

STAT_BENCH_CASE_F(
    nearest_neighbors_search_fixture, "find_neighbors_within_radius", "1D") {
    const auto points =
        num_collect::rbf::generate_1d_halton_nodes<double>(num_points());

    const double radius = 2.0 / static_cast<double>(num_points());
    num_collect::util::vector<std::pair<num_collect::index_type, double>>
        indices_and_distances;

    STAT_BENCH_MEASURE() {
        const num_collect::util::nearest_neighbor_searcher<double> searcher(
            points);

        for (num_collect::index_type i = 0; i < num_points(); ++i) {
            const double& query = points[i];
            searcher.find_neighbors_within_radius(
                radius, query, indices_and_distances);
        }
    };
}

STAT_BENCH_CASE_F(
    nearest_neighbors_search_fixture, "find_k_nearest_neighbors", "2D") {
    constexpr num_collect::index_type dimension = 2;
    const auto points =
        num_collect::rbf::generate_halton_nodes<double, dimension>(
            num_points());

    constexpr num_collect::index_type max_num_neighbors = 10;
    num_collect::util::vector<std::pair<num_collect::index_type, double>>
        indices_and_distances;

    STAT_BENCH_MEASURE() {
        const num_collect::util::nearest_neighbor_searcher<Eigen::Vector2d>
            searcher(points);

        for (num_collect::index_type i = 0; i < num_points(); ++i) {
            const Eigen::Vector2d& query = points[i];
            searcher.find_k_nearest_neighbors(
                max_num_neighbors, query, indices_and_distances);
        }
    };
}

STAT_BENCH_CASE_F(
    nearest_neighbors_search_fixture, "find_neighbors_within_radius", "2D") {
    constexpr num_collect::index_type dimension = 2;
    const auto points =
        num_collect::rbf::generate_halton_nodes<double, dimension>(
            num_points());

    const double radius = 2.0 / std::sqrt(static_cast<double>(num_points()));
    num_collect::util::vector<std::pair<num_collect::index_type, double>>
        indices_and_distances;

    STAT_BENCH_MEASURE() {
        const num_collect::util::nearest_neighbor_searcher<Eigen::Vector2d>
            searcher(points);

        for (num_collect::index_type i = 0; i < num_points(); ++i) {
            const Eigen::Vector2d& query = points[i];
            searcher.find_neighbors_within_radius(
                radius, query, indices_and_distances);
        }
    };
}

STAT_BENCH_CASE_F(
    nearest_neighbors_search_fixture, "find_k_nearest_neighbors", "3D") {
    constexpr num_collect::index_type dimension = 3;
    const auto points =
        num_collect::rbf::generate_halton_nodes<double, dimension>(
            num_points());

    constexpr num_collect::index_type max_num_neighbors = 10;
    num_collect::util::vector<std::pair<num_collect::index_type, double>>
        indices_and_distances;

    STAT_BENCH_MEASURE() {
        const num_collect::util::nearest_neighbor_searcher<Eigen::Vector3d>
            searcher(points);

        for (num_collect::index_type i = 0; i < num_points(); ++i) {
            const Eigen::Vector3d& query = points[i];
            searcher.find_k_nearest_neighbors(
                max_num_neighbors, query, indices_and_distances);
        }
    };
}

STAT_BENCH_CASE_F(
    nearest_neighbors_search_fixture, "find_neighbors_within_radius", "3D") {
    constexpr num_collect::index_type dimension = 3;
    const auto points =
        num_collect::rbf::generate_halton_nodes<double, dimension>(
            num_points());

    const double radius = 2.0 / std::cbrt(static_cast<double>(num_points()));
    num_collect::util::vector<std::pair<num_collect::index_type, double>>
        indices_and_distances;

    STAT_BENCH_MEASURE() {
        const num_collect::util::nearest_neighbor_searcher<Eigen::Vector3d>
            searcher(points);

        for (num_collect::index_type i = 0; i < num_points(); ++i) {
            const Eigen::Vector3d& query = points[i];
            searcher.find_neighbors_within_radius(
                radius, query, indices_and_distances);
        }
    };
}

STAT_BENCH_GROUP("find_k_nearest_neighbors")
    .add_parameter_to_time_line_plot(
        "num_points", stat_bench::PlotOptions().log_parameter(true));
STAT_BENCH_GROUP("find_neighbors_within_radius")
    .add_parameter_to_time_line_plot(
        "num_points", stat_bench::PlotOptions().log_parameter(true));

STAT_BENCH_MAIN
