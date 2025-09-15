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
 * \brief Benchmark of computation of kernel matrices.
 */
#include <vector>

#include <Eigen/Core>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/measurement_config.h>
#include <stat_bench/plot_options.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/impl/compute_kernel_matrix_serial.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/length_parameter_calculators/local_length_parameter_calculator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

class compute_kernel_matrix_fixture : public stat_bench::FixtureBase {
public:
    compute_kernel_matrix_fixture() {
        add_param<num_collect::index_type>("points")
            ->add(10)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(100)   // NOLINT
            ->add(1000)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        num_sample_points_ =
            context.get_param<num_collect::index_type>("points");
        sample_variables_ = num_collect::rbf::generate_halton_nodes<double, 2>(
            num_sample_points_);
    }

    [[nodiscard]] auto variables() const
        -> const std::vector<Eigen::Vector2d>& {
        return sample_variables_;
    }

private:
    num_collect::index_type num_sample_points_{};
    std::vector<Eigen::Vector2d> sample_variables_{};
};

STAT_BENCH_GROUP("compute_kernel_matrix")
    .clear_measurement_configs()
    .add_measurement_config(stat_bench::MeasurementConfig()
            .type("Processing Time")
            .warming_up_samples(1)
            .samples(10)
            .iterations(1))
    .add_parameter_to_time_line_plot(
        "points", stat_bench::PlotOptions().log_parameter(true));

STAT_BENCH_CASE_F(compute_kernel_matrix_fixture, "compute_kernel_matrix",
    "global_rbf_serial") {
    using variable_type = Eigen::Vector2d;
    using distance_function_type =
        num_collect::rbf::distance_functions::euclidean_distance_function<
            variable_type>;
    using rbf_type = num_collect::rbf::rbfs::gaussian_rbf<double>;
    using length_parameter_calculator_type =
        num_collect::rbf::length_parameter_calculators::
            global_length_parameter_calculator<distance_function_type>;
    using kernel_matrix_type = Eigen::MatrixXd;

    distance_function_type distance_function;
    rbf_type rbf;
    length_parameter_calculator_type length_parameter_calculator;
    kernel_matrix_type kernel_matrix;

    STAT_BENCH_MEASURE() {
        num_collect::rbf::impl::compute_kernel_matrix_serial(distance_function,
            rbf, length_parameter_calculator, this->variables(), kernel_matrix);
    };
}

STAT_BENCH_CASE_F(compute_kernel_matrix_fixture, "compute_kernel_matrix",
    "local_rbf_serial") {
    using variable_type = Eigen::Vector2d;
    using distance_function_type =
        num_collect::rbf::distance_functions::euclidean_distance_function<
            variable_type>;
    using rbf_type = num_collect::rbf::rbfs::gaussian_rbf<double>;
    using length_parameter_calculator_type =
        num_collect::rbf::length_parameter_calculators::
            local_length_parameter_calculator<distance_function_type>;
    using kernel_matrix_type = Eigen::MatrixXd;

    distance_function_type distance_function;
    rbf_type rbf;
    length_parameter_calculator_type length_parameter_calculator;
    kernel_matrix_type kernel_matrix;

    STAT_BENCH_MEASURE() {
        num_collect::rbf::impl::compute_kernel_matrix_serial(distance_function,
            rbf, length_parameter_calculator, this->variables(), kernel_matrix);
    };
}

STAT_BENCH_CASE_F(compute_kernel_matrix_fixture, "compute_kernel_matrix",
    "local_csrbf_serial") {
    using variable_type = Eigen::Vector2d;
    using distance_function_type =
        num_collect::rbf::distance_functions::euclidean_distance_function<
            variable_type>;
    using rbf_type = num_collect::rbf::rbfs::wendland_csrbf<double, 3, 1>;
    using length_parameter_calculator_type =
        num_collect::rbf::length_parameter_calculators::
            local_length_parameter_calculator<distance_function_type>;
    using kernel_matrix_type = Eigen::SparseMatrix<double>;

    distance_function_type distance_function;
    rbf_type rbf;
    length_parameter_calculator_type length_parameter_calculator;
    kernel_matrix_type kernel_matrix;

    STAT_BENCH_MEASURE() {
        num_collect::rbf::impl::compute_kernel_matrix_serial(distance_function,
            rbf, length_parameter_calculator, this->variables(), kernel_matrix);
    };
}
