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
 * \brief Benchmark to approximate the Laplacian in 2D using RBF-FD.
 */
#include <cmath>
#include <optional>
#include <stdexcept>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/current_invocation_context.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/measurement_config.h>
#include <stat_bench/plot_options.h>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"

using position_type = Eigen::Vector2d;
using solution_type = Eigen::VectorXd;
using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
using operator_type =
    num_collect::rbf::operators::laplacian_operator<position_type>;

static constexpr num_collect::index_type num_interior_nodes = 100;
static constexpr num_collect::index_type num_boundary_nodes_per_edge = 10;
static constexpr num_collect::index_type num_test_nodes = 10;

class rbf_fd_approximate_laplacian_2d_fixture : public stat_bench::FixtureBase {
public:
    rbf_fd_approximate_laplacian_2d_fixture() {
        add_param<num_collect::index_type>("num_neighbors")
            ->add(10)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(15)
            ->add(20)
            ->add(25)
            ->add(30)
#endif
            ;

        add_param<int>("degree")
            ->add(0)
            ->add(1)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(2)
            ->add(3)
            ->add(4)
            ->add(5)
            ->add(6)
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        num_neighbors_ =
            context.get_param<num_collect::index_type>("num_neighbors");
        degree_ = context.get_param<int>("degree");

        nodes_ = num_collect::rbf::generate_halton_nodes<
            typename position_type::Scalar, position_type::RowsAtCompileTime>(
            num_interior_nodes);
        const auto boundary_nodes =
            num_collect::util::generate_rectangle_boundary_nodes(
                position_type(0.0, 0.0), position_type(1.0, 1.0),
                num_boundary_nodes_per_edge);
        nodes_.insert(
            nodes_.end(), boundary_nodes.begin(), boundary_nodes.end());

        nearest_neighbor_searcher_.emplace(nodes_);

        test_nodes_.clear();
        test_nodes_.reserve(num_test_nodes);
        for (num_collect::index_type i = 0; i < num_test_nodes; ++i) {
            const double x = static_cast<double>(i + 1) / (num_test_nodes + 1);
            test_nodes_.emplace_back(position_type::Constant(x));
        }
    }

    [[nodiscard]] auto num_neighbors() const noexcept
        -> num_collect::index_type {
        return num_neighbors_;
    }

    [[nodiscard]] auto degree() const noexcept -> int { return degree_; }

    [[nodiscard]] auto nodes() const noexcept
        -> num_collect::util::vector_view<const position_type> {
        return nodes_;
    }

    [[nodiscard]] auto nearest_neighbor_searcher() const noexcept
        -> const num_collect::util::nearest_neighbor_searcher<position_type>& {
        return *nearest_neighbor_searcher_;
    }

    [[nodiscard]] auto test_nodes() const noexcept
        -> num_collect::util::vector_view<const position_type> {
        return test_nodes_;
    }

    void evaluate(const sparse_matrix_type& matrix) {
        auto test_function = [](const position_type& position) -> double {
            return (position.array() * num_collect::base::pi<double>)
                .sin()
                .prod();
        };
        auto test_function_laplacian =
            [](const position_type& position) -> double {
            constexpr double coeff = -2.0 * num_collect::base::pi<double> *
                num_collect::base::pi<double>;
            return coeff *
                (position.array() * num_collect::base::pi<double>).sin().prod();
        };

        const solution_type function_values = solution_type::NullaryExpr(
            nodes_.size(), [&](num_collect::index_type i) {
                return test_function(nodes_[i]);
            });
        const solution_type results = matrix * function_values;
        const solution_type expected_results = solution_type::NullaryExpr(
            test_nodes_.size(), [&](num_collect::index_type i) {
                return test_function_laplacian(test_nodes_[i]);
            });
        const double error_rate =
            (results - expected_results).norm() / expected_results.norm();

        stat_bench::current_invocation_context().add_custom_output(
            "error_rate", error_rate);
    }

private:
    num_collect::index_type num_neighbors_{};
    int degree_{};
    num_collect::util::vector<position_type> nodes_{};
    std::optional<num_collect::util::nearest_neighbor_searcher<position_type>>
        nearest_neighbor_searcher_{};
    num_collect::util::vector<position_type> test_nodes_{};
};

STAT_BENCH_GROUP("rbf_fd_approximate_laplacian_2d")
    .add_parameter_to_time_line_plot("num_neighbors")
    .add_parameter_to_time_line_plot("degree")
    .add_parameter_to_output_line_plot("num_neighbors", "error_rate",
        stat_bench::PlotOptions().log_output(true))
    .add_parameter_to_output_line_plot(
        "degree", "error_rate", stat_bench::PlotOptions().log_output(true))
    .add_time_to_output_by_parameter_line_plot("num_neighbors", "error_rate",
        stat_bench::PlotOptions().log_output(true))
    .add_time_to_output_by_parameter_line_plot(
        "degree", "error_rate", stat_bench::PlotOptions().log_output(true));

STAT_BENCH_CASE_F(rbf_fd_approximate_laplacian_2d_fixture,
    "rbf_fd_approximate_laplacian_2d", "phs_rbf_fd_polynomial_assembler") {
    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
    assembler_type assembler(degree());
    if (assembler.min_num_neighbors() > num_neighbors()) {
        throw std::runtime_error("Skip an invalid configuration.");
    }
    assembler.num_neighbors(num_neighbors());
    num_collect::util::vector<Eigen::Triplet<double>> triplets;

    STAT_BENCH_MEASURE() {
        triplets.clear();
        assembler.compute_rows<operator_type>(
            test_nodes(), nodes(), nearest_neighbor_searcher(), triplets);
    };

    sparse_matrix_type matrix(test_nodes().size(), nodes().size());
    matrix.setFromTriplets(triplets.begin(), triplets.end());
    evaluate(matrix);
}
