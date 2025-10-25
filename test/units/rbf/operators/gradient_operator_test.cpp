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
 * \brief Test of gradient_operator class.
 */
#include "num_collect/rbf/operators/gradient_operator.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/rbf_interpolator.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"

TEST_CASE("num_collect::rbf::operators::gradient_operator") {
    using num_collect::index_type;
    using num_collect::rbf::generate_halton_nodes;
    using num_collect::rbf::global_rbf_interpolator;
    using num_collect::rbf::global_rbf_polynomial_interpolator;
    using num_collect::rbf::local_csrbf_interpolator;
    using num_collect::rbf::operators::gradient_operator;
    using num_collect::rbf::operators::operator_evaluator;

    SECTION("evaluate an operator for a one-dimensional variable") {
        using rbf_interpolator_type = global_rbf_interpolator<double(double)>;

        const auto function = [](double x) {
            return std::cos(num_collect::pi<double> * x);
        };
        const auto derivative_function = [](double x) {
            return -num_collect::pi<double> *
                std::sin(num_collect::pi<double> * x);
        };

        const Eigen::VectorXd sample_variables =
            Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables(i));
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        constexpr double evaluated_variable = 0.55;
        const double evaluated_value =
            interpolator.evaluate(gradient_operator(evaluated_variable));
        const double expected_value = derivative_function(evaluated_variable);

        constexpr double tol = 1e-2;
        CHECK_THAT(
            evaluated_value, Catch::Matchers::WithinAbs(expected_value, tol));
    }

    SECTION("evaluate an operator for a one-dimensional variable with CSRBF") {
        using rbf_interpolator_type = local_csrbf_interpolator<double(double)>;

        const auto function = [](double x) {
            return std::cos(num_collect::pi<double> * x);
        };
        const auto derivative_function = [](double x) {
            return -num_collect::pi<double> *
                std::sin(num_collect::pi<double> * x);
        };

        const Eigen::VectorXd sample_variables =
            Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables(i));
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 5.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        constexpr double evaluated_variable = 0.55;
        const double evaluated_value =
            interpolator.evaluate(gradient_operator(evaluated_variable));
        const double expected_value = derivative_function(evaluated_variable);

        constexpr double tol = 1e-2;
        CHECK_THAT(
            evaluated_value, Catch::Matchers::WithinAbs(expected_value, tol));
    }

    SECTION(
        "evaluate an operator for a one-dimensional variable with polynomial "
        "terms") {
        using rbf_interpolator_type =
            global_rbf_polynomial_interpolator<double(double)>;

        const auto function = [](double x) {
            return std::cos(num_collect::pi<double> * x);
        };
        const auto derivative_function = [](double x) {
            return -num_collect::pi<double> *
                std::sin(num_collect::pi<double> * x);
        };

        const Eigen::VectorXd sample_variables =
            Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables(i));
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        constexpr double evaluated_variable = 0.55;
        const double evaluated_value =
            interpolator.evaluate(gradient_operator(evaluated_variable));
        const double expected_value = derivative_function(evaluated_variable);

        constexpr double tol = 1e-2;
        CHECK_THAT(
            evaluated_value, Catch::Matchers::WithinAbs(expected_value, tol));
    }

    SECTION("evaluate an operator for a two-dimensional variable") {
        using rbf_interpolator_type =
            global_rbf_interpolator<double(Eigen::Vector2d)>;

        const auto function = [](const Eigen::Vector2d& variable) {
            return variable.squaredNorm();
        };
        const auto derivative_function = [](const Eigen::Vector2d& variable) {
            return 2.0 * variable;
        };

        const auto sample_variables = generate_halton_nodes<double, 2>(100);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables[i]);
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::Vector2d evaluated_variable(0.3, 0.4);
        const Eigen::Vector2d evaluated_value =
            interpolator.evaluate(gradient_operator(evaluated_variable));
        const Eigen::Vector2d expected_value =
            derivative_function(evaluated_variable);

        constexpr double tol = 1e-2;
        CHECK_THAT(evaluated_value, eigen_approx(expected_value, tol));
    }

    SECTION("evaluate an operator for a two-dimensional variable with CSRBF") {
        using rbf_interpolator_type =
            local_csrbf_interpolator<double(Eigen::Vector2d)>;

        const auto function = [](const Eigen::Vector2d& variable) {
            return variable.squaredNorm();
        };
        const auto derivative_function = [](const Eigen::Vector2d& variable) {
            return 2.0 * variable;
        };

        const auto sample_variables = generate_halton_nodes<double, 2>(100);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables[i]);
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 10.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::Vector2d evaluated_variable(0.3, 0.4);
        const Eigen::Vector2d evaluated_value =
            interpolator.evaluate(gradient_operator(evaluated_variable));
        const Eigen::Vector2d expected_value =
            derivative_function(evaluated_variable);

        constexpr double tol = 1e-2;
        CHECK_THAT(evaluated_value, eigen_approx(expected_value, tol));
    }

    SECTION(
        "evaluate an operator for a two-dimensional variable with polynomial "
        "terms") {
        using rbf_interpolator_type =
            global_rbf_polynomial_interpolator<double(Eigen::Vector2d)>;

        const auto function = [](const Eigen::Vector2d& variable) {
            return variable.squaredNorm();
        };
        const auto derivative_function = [](const Eigen::Vector2d& variable) {
            return 2.0 * variable;
        };

        const auto sample_variables = generate_halton_nodes<double, 2>(100);
        Eigen::VectorXd sample_values{};
        sample_values.resize(sample_variables.size());
        for (index_type i = 0; i < sample_variables.size(); ++i) {
            sample_values(i) = function(sample_variables[i]);
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::Vector2d evaluated_variable(0.3, 0.4);
        const Eigen::Vector2d evaluated_value =
            interpolator.evaluate(gradient_operator(evaluated_variable));
        const Eigen::Vector2d expected_value =
            derivative_function(evaluated_variable);

        constexpr double tol = 1e-2;
        CHECK_THAT(evaluated_value, eigen_approx(expected_value, tol));
    }
}
