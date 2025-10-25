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
 * \brief Test of third_order_derivative_operator class.
 */
#include "num_collect/rbf/operators/third_order_derivative_operator.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/rbf_interpolator.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

TEST_CASE("num_collect::rbf::operators::third_order_derivative_operator") {
    using num_collect::index_type;
    using num_collect::rbf::generate_1d_halton_nodes;
    using num_collect::rbf::global_rbf_interpolator;
    using num_collect::rbf::local_csrbf_interpolator;
    using num_collect::rbf::operators::operator_evaluator;
    using num_collect::rbf::operators::third_order_derivative_operator;
    using num_collect::rbf::rbfs::wendland_csrbf;

    SECTION("evaluate an operator for a one-dimensional variable") {
        using rbf_interpolator_type = global_rbf_interpolator<double(double)>;

        const auto function = [](double x) {
            return std::cos(num_collect::pi<double> * x);
        };
        const auto third_derivative_function = [](double x) {
            return num_collect::pi<double> * num_collect::pi<double> *
                num_collect::pi<double> * std::sin(num_collect::pi<double> * x);
        };

        const auto sample_variables = generate_1d_halton_nodes<double>(30);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (num_collect::index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables[i]);
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 10.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        constexpr double evaluated_variable = 0.50;
        const double evaluated_value = interpolator.evaluate(
            third_order_derivative_operator(evaluated_variable));
        const double expected_value =
            third_derivative_function(evaluated_variable);

        constexpr double tol = 1e-2;
        CHECK_THAT(
            evaluated_value, Catch::Matchers::WithinAbs(expected_value, tol));
    }

    SECTION("evaluate an operator for a one-dimensional variable with CSRBF") {
        using rbf_interpolator_type = local_csrbf_interpolator<double(double),
            wendland_csrbf<double, 3, 2>>;

        const auto function = [](double x) {
            return std::cos(num_collect::pi<double> * x);
        };
        const auto third_derivative_function = [](double x) {
            return num_collect::pi<double> * num_collect::pi<double> *
                num_collect::pi<double> * std::sin(num_collect::pi<double> * x);
        };

        const auto sample_variables = generate_1d_halton_nodes<double>(30);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (num_collect::index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables[i]);
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 10.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        constexpr double evaluated_variable = 0.50;
        const double evaluated_value = interpolator.evaluate(
            third_order_derivative_operator(evaluated_variable));
        const double expected_value =
            third_derivative_function(evaluated_variable);

        // Precision is bad for this case.
        constexpr double tol = 10.0;
        CHECK_THAT(
            evaluated_value, Catch::Matchers::WithinAbs(expected_value, tol));
    }
}
