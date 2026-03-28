/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Benchmark to assemble matrix of Laplacian in 2D by RBF-FD method.
 */
#include <cmath>
#include <optional>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/measurement_config.h>
#include <stat_bench/plot_option.h>
#include <stat_bench/plot_options.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_assembler.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"

using variable_type = Eigen::Vector2d;

class rbf_fd_assemble_laplacian_2d_fixture : public stat_bench::FixtureBase {
public:
    rbf_fd_assemble_laplacian_2d_fixture() {
        add_param<num_collect::index_type>("nodes")
            ->add(100)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(1000)
            ->add(10000)
#endif
            ;
        add_param<num_collect::index_type>("neighbors")
            ->add(10)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(15)
            ->add(20)
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        num_interior_nodes_ =
            context.get_param<num_collect::index_type>("nodes");
        num_neighbors_ =
            context.get_param<num_collect::index_type>("neighbors");
        const auto num_boundary_nodes_per_edge =
            static_cast<num_collect::index_type>(
                std::sqrt(num_interior_nodes_));

        const auto boundary_nodes =
            num_collect::util::generate_rectangle_boundary_nodes(
                Eigen::Vector2d(0.0, 0.0), Eigen::Vector2d(1.0, 1.0),
                num_boundary_nodes_per_edge);
        const auto interior_nodes =
            num_collect::rbf::generate_halton_nodes<double, 2>(
                num_interior_nodes_);
        nodes_.clear();
        nodes_.reserve(interior_nodes.size() + boundary_nodes.size());
        nodes_.insert(
            nodes_.end(), interior_nodes.begin(), interior_nodes.end());
        nodes_.insert(
            nodes_.end(), boundary_nodes.begin(), boundary_nodes.end());

        nearest_neighbor_searcher_.emplace(nodes_);
    }

    [[nodiscard]] auto num_interior_nodes() const noexcept
        -> num_collect::index_type {
        return num_interior_nodes_;
    }

    [[nodiscard]] auto num_neighbors() const noexcept
        -> num_collect::index_type {
        return num_neighbors_;
    }

    [[nodiscard]] auto nodes() const noexcept
        -> num_collect::util::vector_view<const variable_type> {
        return nodes_;
    }

    [[nodiscard]] auto interior_nodes() const noexcept
        -> num_collect::util::vector_view<const variable_type> {
        return nodes().first(num_interior_nodes_);
    }

    [[nodiscard]] auto nearest_neighbor_searcher() const noexcept
        -> const num_collect::util::nearest_neighbor_searcher<variable_type>& {
        return *nearest_neighbor_searcher_;
    }

private:
    num_collect::index_type num_interior_nodes_{};
    num_collect::index_type num_neighbors_{};
    num_collect::util::vector<variable_type> nodes_{};
    std::optional<num_collect::util::nearest_neighbor_searcher<variable_type>>
        nearest_neighbor_searcher_{};
};

STAT_BENCH_GROUP("rbf_fd_assemble_laplacian_2d")
    .clear_measurement_configs()
    .add_measurement_config(stat_bench::MeasurementConfig()
            .type("Processing Time")
            .iterations(1)
            .samples(30)
            .warming_up_samples(1))
    .add_parameter_to_time_line_plot(
        "nodes", stat_bench::PlotOptions().log_parameter(true));

STAT_BENCH_MAIN

STAT_BENCH_CASE_F(rbf_fd_assemble_laplacian_2d_fixture,
    "rbf_fd_assemble_laplacian_2d", "rbf_fd_assembler") {
    num_collect::rbf::rbf_fd_assembler<variable_type> assembler;
    assembler.num_neighbors(num_neighbors());
    STAT_BENCH_MEASURE() {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        assembler.compute_rows<
            num_collect::rbf::operators::laplacian_operator<variable_type>>(
            interior_nodes(), nodes(), nearest_neighbor_searcher(), triplets, 0,
            0);
    };
}

STAT_BENCH_CASE_F(rbf_fd_assemble_laplacian_2d_fixture,
    "rbf_fd_assemble_laplacian_2d", "rbf_fd_polynomial_assembler") {
    num_collect::rbf::rbf_fd_polynomial_assembler<variable_type> assembler;
    assembler.num_neighbors(num_neighbors());
    STAT_BENCH_MEASURE() {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        assembler.compute_rows<
            num_collect::rbf::operators::laplacian_operator<variable_type>>(
            interior_nodes(), nodes(), nearest_neighbor_searcher(), triplets, 0,
            0);
    };
}

STAT_BENCH_CASE_F(rbf_fd_assemble_laplacian_2d_fixture,
    "rbf_fd_assemble_laplacian_2d", "phs_rbf_fd_polynomial_assembler") {
    num_collect::rbf::phs_rbf_fd_polynomial_assembler<variable_type> assembler;
    assembler.num_neighbors(num_neighbors());
    STAT_BENCH_MEASURE() {
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        assembler.compute_rows<
            num_collect::rbf::operators::laplacian_operator<variable_type>>(
            interior_nodes(), nodes(), nearest_neighbor_searcher(), triplets, 0,
            0);
    };
}
