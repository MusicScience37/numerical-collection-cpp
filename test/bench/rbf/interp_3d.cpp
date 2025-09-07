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
 * \brief Benchmark of RBF interpolation in 3 dimensions.
 */
#include <cmath>
#include <cstddef>
#include <vector>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/plot_option.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/rbf_interpolator.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

class interpolate_3d_fixture_base : public stat_bench::FixtureBase {
public:
    interpolate_3d_fixture_base() = default;

    void setup(stat_bench::InvocationContext& context) override {
        num_sample_points_ =
            context.get_param<num_collect::index_type>("points");

        constexpr double max_variable = 3.0;
        constexpr double min_variable = -3.0;

        sample_variables_ = num_collect::rbf::generate_halton_nodes<double, 3>(
            num_sample_points_);
        for (auto& variable : sample_variables_) {
            variable = Eigen::Vector3d::Constant(min_variable) +
                (max_variable - min_variable) * variable;
        }
        sample_values_.resize(num_sample_points_);
        for (num_collect::index_type i = 0; i < num_sample_points_; ++i) {
            sample_values_[i] =
                function(sample_variables_[static_cast<std::size_t>(i)]);
        }

        num_evaluation_points_ = 100;  // NOLINT
        evaluation_variables_.resize(
            static_cast<std::size_t>(num_evaluation_points_));
        evaluation_correct_values_.resize(num_evaluation_points_);
        evaluation_interpolated_values_.resize(num_evaluation_points_);
        for (num_collect::index_type i = 0; i < num_evaluation_points_; ++i) {
            const double rate = static_cast<double>(i + 1) /
                static_cast<double>(num_evaluation_points_ + 1);
            const double variable_element =
                min_variable + (max_variable - min_variable) * rate;
            const Eigen::Vector3d variable =
                Eigen::Vector3d::Constant(variable_element);
            evaluation_variables_[static_cast<std::size_t>(i)] = variable;
            evaluation_correct_values_[i] = function(variable);
            evaluation_interpolated_values_[i] = 0.0;
        }
    }

    template <typename Interpolator>
    void perform(Interpolator& interpolator) {
        interpolator.compute(sample_variables_, sample_values_);
        for (num_collect::index_type i = 0; i < num_evaluation_points_; ++i) {
            const auto& variable =
                evaluation_variables_[static_cast<std::size_t>(i)];
            evaluation_interpolated_values_[i] =
                interpolator.interpolate(variable);
        }
    }

    template <typename Interpolator>
    void perform_with_optimization(Interpolator& interpolator) {
        interpolator.optimize_length_parameter_scale(
            sample_variables_, sample_values_);
        interpolator.compute(sample_variables_, sample_values_);
        for (num_collect::index_type i = 0; i < num_evaluation_points_; ++i) {
            const auto& variable =
                evaluation_variables_[static_cast<std::size_t>(i)];
            evaluation_interpolated_values_[i] =
                interpolator.interpolate(variable);
        }
    }

    void tear_down(stat_bench::InvocationContext& context) override {
        const double error_rate =
            (evaluation_correct_values_ - evaluation_interpolated_values_)
                .norm() /
            evaluation_correct_values_.norm();
        context.add_custom_output("error_rate", error_rate);
    }

    static auto function(const Eigen::Vector3d& variable) -> double {
        return std::cos(variable.x()) + std::sin(variable.y() + variable.z());
    }

private:
    num_collect::index_type num_sample_points_{};

    std::vector<Eigen::Vector3d> sample_variables_{};

    Eigen::VectorXd sample_values_{};

    num_collect::index_type num_evaluation_points_{};

    std::vector<Eigen::Vector3d> evaluation_variables_{};

    Eigen::VectorXd evaluation_correct_values_{};

    Eigen::VectorXd evaluation_interpolated_values_{};
};

class interpolate_3d_fixture_light : public interpolate_3d_fixture_base {
public:
    interpolate_3d_fixture_light() {
        add_param<num_collect::index_type>("points")
            ->add(10)  // NOLINT
            ->add(20)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(50)   // NOLINT
            ->add(100)  // NOLINT
#endif
            ;
    }
};

class interpolate_3d_fixture_medium : public interpolate_3d_fixture_base {
public:
    interpolate_3d_fixture_medium() {
        add_param<num_collect::index_type>("points")
            ->add(10)  // NOLINT
            ->add(20)  // NOLINT
            ->add(50)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(100)  // NOLINT
            ->add(200)  // NOLINT
            ->add(500)  // NOLINT
#endif
            ;
    }
};

class interpolate_3d_fixture_large : public interpolate_3d_fixture_base {
public:
    interpolate_3d_fixture_large() {
        add_param<num_collect::index_type>("points")
            ->add(10)  // NOLINT
            ->add(20)  // NOLINT
            ->add(50)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(100)   // NOLINT
            ->add(200)   // NOLINT
            ->add(500)   // NOLINT
            ->add(1000)  // NOLINT
#endif
            ;
    }
};

STAT_BENCH_GROUP("interpolate_3d")
    .add_parameter_to_time_line_plot(
        "points", stat_bench::PlotOption::log_parameter)
    .add_parameter_to_output_line_plot("points", "error_rate",
        stat_bench::PlotOption::log_parameter |
            stat_bench::PlotOption::log_output)
    .add_time_to_output_by_parameter_line_plot(
        "points", "error_rate", stat_bench::PlotOption::log_output);

STAT_BENCH_CASE_F(interpolate_3d_fixture_medium, "interpolate_3d",
    "global_rbf_interpolator") {
    STAT_BENCH_MEASURE() {
        num_collect::rbf::global_rbf_interpolator<double(Eigen::Vector3d)>
            interpolator;
        perform(interpolator);
    };
}

STAT_BENCH_CASE_F(interpolate_3d_fixture_light, "interpolate_3d",
    "global_rbf_interpolator (with optimization)") {
    STAT_BENCH_MEASURE() {
        num_collect::rbf::global_rbf_interpolator<double(Eigen::Vector3d)>
            interpolator;
        perform_with_optimization(interpolator);
    };
}

STAT_BENCH_CASE_F(
    interpolate_3d_fixture_large, "interpolate_3d", "local_rbf_interpolator") {
    STAT_BENCH_MEASURE() {
        num_collect::rbf::local_rbf_interpolator<double(Eigen::Vector3d)>
            interpolator;
        perform(interpolator);
    };
}

STAT_BENCH_CASE_F(interpolate_3d_fixture_large, "interpolate_3d",
    "local_csrbf_interpolator") {
    STAT_BENCH_MEASURE() {
        num_collect::rbf::local_csrbf_interpolator<double(Eigen::Vector3d)>
            interpolator;
        perform(interpolator);
    };
}

STAT_BENCH_CASE_F(interpolate_3d_fixture_medium, "interpolate_3d",
    "global_rbf_polynomial_interpolator(0)") {
    STAT_BENCH_MEASURE() {
        num_collect::rbf::global_rbf_polynomial_interpolator<
            double(Eigen::Vector3d),
            num_collect::rbf::rbfs::gaussian_m1_rbf<double>, 0>
            interpolator;
        perform(interpolator);
    };
}

STAT_BENCH_CASE_F(interpolate_3d_fixture_medium, "interpolate_3d",
    "global_rbf_polynomial_interpolator(1)") {
    STAT_BENCH_MEASURE() {
        num_collect::rbf::global_rbf_polynomial_interpolator<
            double(Eigen::Vector3d),
            num_collect::rbf::rbfs::gaussian_m1_rbf<double>, 1>
            interpolator;
        perform(interpolator);
    };
}
