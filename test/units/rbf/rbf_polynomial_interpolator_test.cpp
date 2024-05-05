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
 * \brief Test of rbf_polynomial_interpolator class.
 */
#include "num_collect/rbf/rbf_polynomial_interpolator.h"

#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

TEST_CASE(
    "num_collect::rbf::global_rbf_polynomial_interpolator "
    "(for scalar variable)") {
    using num_collect::rbf::global_rbf_polynomial_interpolator;
    using num_collect::rbf::rbfs::gaussian_rbf;

    using rbf_interpolator_type =
        global_rbf_polynomial_interpolator<double(double)>;

    rbf_interpolator_type interpolator;

    SECTION("interpolate with a fixed scale") {
        const auto function = [](double x) {
            return std::cos(num_collect::constants::pi<double> * x);
        };

        const auto sample_variables = std::vector<double>{0.0, 0.5, 0.8, 1.0};
        Eigen::VectorXd sample_values{};
        sample_values.resize(
            static_cast<num_collect::index_type>(sample_variables.size()));
        for (std::size_t i = 0; i < sample_variables.size(); ++i) {
            sample_values(static_cast<num_collect::index_type>(i)) =
                function(sample_variables[i]);
        }

        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::VectorXd interpolated_variables =
            Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
        Eigen::VectorXd interpolated_values;
        interpolated_values.resize(interpolated_variables.size());
        Eigen::VectorXd actual_values;
        actual_values.resize(interpolated_variables.size());
        for (num_collect::index_type i = 0; i < interpolated_variables.size();
             ++i) {
            interpolated_values(i) =
                interpolator.interpolate(interpolated_variables(i));
            actual_values(i) = function(interpolated_variables(i));
        }
        comparison_approvals::verify_with_reference(
            interpolated_values, actual_values, 2);
    }

    SECTION("interpolate with a optimal scale") {
        const auto function = [](double x) {
            return std::cos(num_collect::constants::pi<double> * x);
        };

        const auto sample_variables = std::vector<double>{0.0, 0.5, 0.8, 1.0};
        Eigen::VectorXd sample_values{};
        sample_values.resize(
            static_cast<num_collect::index_type>(sample_variables.size()));
        for (std::size_t i = 0; i < sample_variables.size(); ++i) {
            sample_values(static_cast<num_collect::index_type>(i)) =
                function(sample_variables[i]);
        }

        interpolator.optimize_length_parameter_scale(
            sample_variables, sample_values);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::VectorXd interpolated_variables =
            Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
        Eigen::VectorXd interpolated_values;
        interpolated_values.resize(interpolated_variables.size());
        Eigen::VectorXd actual_values;
        actual_values.resize(interpolated_variables.size());
        for (num_collect::index_type i = 0; i < interpolated_variables.size();
             ++i) {
            interpolated_values(i) =
                interpolator.interpolate(interpolated_variables(i));
            actual_values(i) = function(interpolated_variables(i));
        }
        comparison_approvals::verify_with_reference(
            interpolated_values, actual_values, 2);
    }
}

TEST_CASE(
    "num_collect::rbf::global_rbf_polynomial_interpolator "
    "(for vector variable)") {
    using num_collect::rbf::global_rbf_polynomial_interpolator;
    using num_collect::rbf::rbfs::gaussian_rbf;

    using rbf_interpolator_type =
        global_rbf_polynomial_interpolator<double(Eigen::Vector2d)>;

    rbf_interpolator_type interpolator;

    const auto function = [](const Eigen::Vector2d& var) {
        return var.squaredNorm();
    };

    std::vector<Eigen::Vector2d> sample_variables;
    for (std::size_t i = 0; i < 3; ++i) {
        const double x = static_cast<double>(i) / static_cast<double>(2);
        for (std::size_t j = 0; j < 3; ++j) {
            const double y = static_cast<double>(j) / static_cast<double>(2);
            sample_variables.emplace_back(x, y);
        }
    }
    Eigen::VectorXd sample_values{};
    sample_values.resize(
        static_cast<num_collect::index_type>(sample_variables.size()));
    for (std::size_t i = 0; i < sample_variables.size(); ++i) {
        sample_values(static_cast<num_collect::index_type>(i)) =
            function(sample_variables[i]);
    }

    std::vector<Eigen::Vector2d> interpolated_variables;
    for (std::size_t i = 0; i < 5; ++i) {  // NOLINT
        const double x = static_cast<double>(i) / static_cast<double>(4);
        for (std::size_t j = 0; j < 5; ++j) {  // NOLINT
            const double y = static_cast<double>(j) / static_cast<double>(4);
            interpolated_variables.emplace_back(x, y);
        }
    }

    SECTION("interpolate with a fixed scale") {
        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        Eigen::VectorXd interpolated_values;
        interpolated_values.resize(static_cast<num_collect::index_type>(
            interpolated_variables.size()));
        Eigen::VectorXd actual_values;
        actual_values.resize(static_cast<num_collect::index_type>(
            interpolated_variables.size()));
        for (std::size_t i = 0; i < interpolated_variables.size(); ++i) {
            interpolated_values(static_cast<num_collect::index_type>(i)) =
                interpolator.interpolate(interpolated_variables[i]);
            actual_values(static_cast<num_collect::index_type>(i)) =
                function(interpolated_variables[i]);
        }
        comparison_approvals::verify_with_reference(
            interpolated_values, actual_values, 2);
    }

    SECTION("interpolate with a optimal scale") {
        interpolator.optimize_length_parameter_scale(
            sample_variables, sample_values);
        interpolator.compute(sample_variables, sample_values);

        Eigen::VectorXd interpolated_values;
        interpolated_values.resize(static_cast<num_collect::index_type>(
            interpolated_variables.size()));
        Eigen::VectorXd actual_values;
        actual_values.resize(static_cast<num_collect::index_type>(
            interpolated_variables.size()));
        for (std::size_t i = 0; i < interpolated_variables.size(); ++i) {
            interpolated_values(static_cast<num_collect::index_type>(i)) =
                interpolator.interpolate(interpolated_variables[i]);
            actual_values(static_cast<num_collect::index_type>(i)) =
                function(interpolated_variables[i]);
        }
        comparison_approvals::verify_with_reference(
            interpolated_values, actual_values, 2);
    }
}
