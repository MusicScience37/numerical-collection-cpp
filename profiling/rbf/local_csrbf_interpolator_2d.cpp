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
 * \brief Profiling of local_csrbf_interpolator in 2D.
 */

#include <vector>

#include <Eigen/Core>
#include <gperftools/profiler.h>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/rbf_interpolator.h"

constexpr num_collect::index_type num_sample_points = 100;
constexpr num_collect::index_type num_evaluation_points = 100;

static auto function(const Eigen::Vector2d& variable) -> double {
    return variable.x() * variable.x() + std::sin(variable.y());
}

static void test(const std::vector<Eigen::Vector2d>& sample_variables,
    const Eigen::VectorXd& sample_values,
    const std::vector<Eigen::Vector2d>& evaluation_variables,
    Eigen::VectorXd& evaluation_interpolated_values) {
    num_collect::rbf::local_csrbf_interpolator<double(Eigen::Vector2d)>
        interpolator;
    interpolator.compute(sample_variables, sample_values);
    for (num_collect::index_type i = 0; i < num_evaluation_points; ++i) {
        const auto& variable =
            evaluation_variables[static_cast<std::size_t>(i)];
        evaluation_interpolated_values[i] = interpolator.interpolate(variable);
    }
}

auto main() -> int {
    constexpr double max_variable = 3.0;
    constexpr double min_variable = -3.0;

    std::vector<Eigen::Vector2d> sample_variables =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_sample_points);
    for (auto& variable : sample_variables) {
        variable = Eigen::Vector2d::Constant(min_variable) +
            (max_variable - min_variable) * variable;
    }

    Eigen::VectorXd sample_values;
    sample_values.resize(num_sample_points);
    for (num_collect::index_type i = 0; i < num_sample_points; ++i) {
        sample_values[i] =
            function(sample_variables[static_cast<std::size_t>(i)]);
    }

    std::vector<Eigen::Vector2d> evaluation_variables;
    evaluation_variables.resize(
        static_cast<std::size_t>(num_evaluation_points));
    Eigen::VectorXd evaluation_interpolated_values;
    evaluation_interpolated_values.resize(num_evaluation_points);
    for (num_collect::index_type i = 0; i < num_evaluation_points; ++i) {
        const double rate = static_cast<double>(i + 1) /
            static_cast<double>(num_evaluation_points + 1);
        const double variable_element =
            min_variable + (max_variable - min_variable) * rate;
        const Eigen::Vector2d variable =
            Eigen::Vector2d::Constant(variable_element);
        evaluation_variables[static_cast<std::size_t>(i)] = variable;
        evaluation_interpolated_values[i] = 0.0;
    }

    constexpr std::size_t repetition = 1000;
    ProfilerStart("profile_rbf_local_csrbf_interpolator_2d.prof");
    for (std::size_t i = 0; i < repetition; ++i) {
        test(sample_variables, sample_values, evaluation_variables,
            evaluation_interpolated_values);
    }
    ProfilerStop();
    return 0;
}
