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
 * \brief Test of polyharmonic_operator class.
 */
#include "num_collect/rbf/operators/polyharmonic_operator.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"
#include "num_collect/rbf/rbfs/polyharmonic_spline_rbf.h"

TEST_CASE("num_collect::rbf::operators::polyharmonic_operator") {
    using num_collect::index_type;
    using num_collect::rbf::generate_1d_halton_nodes;
    using num_collect::rbf::generate_halton_nodes;
    using num_collect::rbf::global_rbf_polynomial_interpolator;
    using num_collect::rbf::operators::polyharmonic_operator;
    using num_collect::rbf::rbfs::polyharmonic_spline_rbf;

    SECTION("order 1") {
        constexpr int polyharmonic_order = 1;

        // Polyharmonic operator only supports polyharmonic spline RBFs,
        // and polyharmonic spline RBFs only support interpolation with
        // polynomials terms.

        SECTION(
            "evaluate an operator for a one-dimensional variable with "
            "polynomial terms") {
            using rbf_interpolator_type =
                global_rbf_polynomial_interpolator<double(double),
                    polyharmonic_spline_rbf<double, 7>, 6>;

            const auto function = [](double x) {
                return std::cos(num_collect::pi<double> * x);
            };
            const auto second_derivative_function = [](double x) {
                return -num_collect::pi<double> * num_collect::pi<double> *
                    std::cos(num_collect::pi<double> * x);
            };

            const Eigen::VectorXd sample_variables =
                Eigen::VectorXd::LinSpaced(101, 0.0, 1.0);
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
            const double evaluated_value = interpolator.evaluate(
                polyharmonic_operator<polyharmonic_order, double>(
                    evaluated_variable));
            const double expected_value =
                second_derivative_function(evaluated_variable);

            constexpr double tol = 1e-2;
            CHECK_THAT(evaluated_value,
                Catch::Matchers::WithinAbs(expected_value, tol));
        }

        SECTION(
            "evaluate an operator for a two-dimensional variable with "
            "polynomial terms") {
            using rbf_interpolator_type =
                global_rbf_polynomial_interpolator<double(Eigen::Vector2d),
                    polyharmonic_spline_rbf<double, 7>, 6>;

            const auto function = [](const Eigen::Vector2d& variable) {
                return variable.squaredNorm();
            };
            const auto second_derivative_function =
                [](const Eigen::Vector2d& /*variable*/) { return 4.0; };

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
            const double evaluated_value = interpolator.evaluate(
                polyharmonic_operator<polyharmonic_order, Eigen::Vector2d>(
                    evaluated_variable));
            const double expected_value =
                second_derivative_function(evaluated_variable);

            constexpr double tol = 1e-2;
            CHECK_THAT(evaluated_value,
                Catch::Matchers::WithinAbs(expected_value, tol));
        }
    }

    SECTION("order 2") {
        constexpr int polyharmonic_order = 2;

        // Polyharmonic operator only supports polyharmonic spline RBFs,
        // and polyharmonic spline RBFs only support interpolation with
        // polynomials terms.

        SECTION(
            "evaluate an operator for a one-dimensional variable with "
            "polynomial terms") {
            using rbf_interpolator_type =
                global_rbf_polynomial_interpolator<double(double),
                    polyharmonic_spline_rbf<double, 7>, 6>;

            const auto function = [](double x) {
                return std::cos(num_collect::pi<double> * x);
            };
            const auto fourth_derivative_function = [](double x) {
                return num_collect::pi<double> * num_collect::pi<double> *
                    num_collect::pi<double> * num_collect::pi<double> *
                    std::cos(num_collect::pi<double> * x);
            };

            const auto sample_variables = generate_1d_halton_nodes<double>(100);
            Eigen::VectorXd sample_values{};
            sample_values.resize(sample_variables.size());
            for (index_type i = 0; i < sample_variables.size(); ++i) {
                sample_values(i) = function(sample_variables[i]);
            }

            rbf_interpolator_type interpolator;
            constexpr double length_parameter_scale = 2.0;
            interpolator.fix_length_parameter_scale(length_parameter_scale);
            interpolator.compute(sample_variables, sample_values);

            constexpr double evaluated_variable = 0.55;
            const double evaluated_value = interpolator.evaluate(
                polyharmonic_operator<polyharmonic_order, double>(
                    evaluated_variable));
            const double expected_value =
                fourth_derivative_function(evaluated_variable);

            constexpr double tol = 1e-2;
            CHECK_THAT(evaluated_value,
                Catch::Matchers::WithinAbs(expected_value, tol));
        }

        SECTION(
            "evaluate an operator for a two-dimensional variable with "
            "polynomial terms") {
            using rbf_interpolator_type =
                global_rbf_polynomial_interpolator<double(Eigen::Vector2d),
                    polyharmonic_spline_rbf<double, 7>, 6>;

            const auto function = [](const Eigen::Vector2d& variable) {
                return variable.array().sin().prod();
            };
            const auto fourth_derivative_function =
                [](const Eigen::Vector2d& variable) {
                    return 4.0 * variable.array().sin().prod();
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
            const double evaluated_value = interpolator.evaluate(
                polyharmonic_operator<polyharmonic_order, Eigen::Vector2d>(
                    evaluated_variable));
            const double expected_value =
                fourth_derivative_function(evaluated_variable);

            constexpr double tol = 1e-2;
            CHECK_THAT(evaluated_value,
                Catch::Matchers::WithinAbs(expected_value, tol));
        }
    }

    SECTION("order 3") {
        constexpr int polyharmonic_order = 3;

        // Polyharmonic operator only supports polyharmonic spline RBFs,
        // and polyharmonic spline RBFs only support interpolation with
        // polynomials terms.

        SECTION(
            "evaluate an operator for a one-dimensional variable with "
            "polynomial terms") {
            using rbf_interpolator_type =
                global_rbf_polynomial_interpolator<double(double),
                    polyharmonic_spline_rbf<double, 7>, 6>;

            const auto function = [](double x) {
                return std::cos(num_collect::pi<double> * x);
            };
            const auto sixth_derivative_function = [](double x) {
                return -num_collect::pi<double> * num_collect::pi<double> *
                    num_collect::pi<double> * num_collect::pi<double> *
                    num_collect::pi<double> * num_collect::pi<double> *
                    std::cos(num_collect::pi<double> * x);
            };

            const auto sample_variables = generate_1d_halton_nodes<double>(100);
            Eigen::VectorXd sample_values{};
            sample_values.resize(sample_variables.size());
            for (index_type i = 0; i < sample_variables.size(); ++i) {
                sample_values(i) = function(sample_variables[i]);
            }

            rbf_interpolator_type interpolator;
            constexpr double length_parameter_scale = 2.0;
            interpolator.fix_length_parameter_scale(length_parameter_scale);
            interpolator.compute(sample_variables, sample_values);

            constexpr double evaluated_variable = 0.55;
            const double evaluated_value = interpolator.evaluate(
                polyharmonic_operator<polyharmonic_order, double>(
                    evaluated_variable));
            const double expected_value =
                sixth_derivative_function(evaluated_variable);

#if defined(__GNUC__) && !defined(__clang__)
            // Only GCC has a large error.
            constexpr double tol = 100.0;
#else
            constexpr double tol = 10.0;
#endif
            CHECK_THAT(evaluated_value,
                Catch::Matchers::WithinAbs(expected_value, tol));
        }

        SECTION(
            "evaluate an operator for a two-dimensional variable with "
            "polynomial terms") {
            using rbf_interpolator_type =
                global_rbf_polynomial_interpolator<double(Eigen::Vector2d),
                    polyharmonic_spline_rbf<double, 7>, 6>;

            const auto function = [](const Eigen::Vector2d& variable) {
                return variable.array().sin().prod();
            };
            const auto sixth_derivative_function =
                [](const Eigen::Vector2d& variable) {
                    return -8.0 * variable.array().sin().prod();
                };

            const auto sample_variables = generate_halton_nodes<double, 2>(200);
            Eigen::VectorXd sample_values{};
            sample_values.resize(sample_variables.size());
            for (index_type i = 0; i < sample_variables.size(); ++i) {
                sample_values(i) = function(sample_variables[i]);
            }

            rbf_interpolator_type interpolator;
            constexpr double length_parameter_scale = 2.0;
            interpolator.fix_length_parameter_scale(length_parameter_scale);
            interpolator.compute(sample_variables, sample_values);

            const Eigen::Vector2d evaluated_variable(0.7, 0.8);
            const double evaluated_value = interpolator.evaluate(
                polyharmonic_operator<polyharmonic_order, Eigen::Vector2d>(
                    evaluated_variable));
            const double expected_value =
                sixth_derivative_function(evaluated_variable);

            constexpr double tol = 1.0;
            CHECK_THAT(evaluated_value,
                Catch::Matchers::WithinAbs(expected_value, tol));
        }
    }
}
