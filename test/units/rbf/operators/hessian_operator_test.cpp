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
 * \brief Test of hessian_operator class.
 */
#include "num_collect/rbf/operators/hessian_operator.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/rbf_interpolator.h"

TEST_CASE("num_collect::rbf::operators::hessian_operator") {
    using num_collect::index_type;
    using num_collect::rbf::generate_halton_nodes;
    using num_collect::rbf::global_rbf_interpolator;
    using num_collect::rbf::local_csrbf_interpolator;
    using num_collect::rbf::operators::hessian_operator;
    using num_collect::rbf::operators::operator_evaluator;

    SECTION("evaluate an operator for a two-dimensional variable") {
        using rbf_interpolator_type =
            global_rbf_interpolator<double(Eigen::Vector2d)>;

        const auto function = [](const Eigen::Vector2d& variable) {
            return variable.squaredNorm();
        };
        const auto second_derivative_function =
            [](const Eigen::Vector2d& /*variable*/) {
                // NOLINTNEXTLINE(*-magic-numbers)
                return Eigen::Matrix2d{{2.0, 0.0}, {0.0, 2.0}};
            };

        const auto sample_variables = generate_halton_nodes<double, 2>(100);
        Eigen::VectorXd sample_values{};
        sample_values.resize(static_cast<index_type>(sample_variables.size()));
        for (std::size_t i = 0; i < sample_variables.size(); ++i) {
            sample_values(static_cast<index_type>(i)) =
                function(sample_variables[i]);
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::Vector2d evaluated_variable(0.3, 0.4);
        const Eigen::Matrix2d evaluated_value =
            interpolator.evaluate(hessian_operator(evaluated_variable));
        const Eigen::Matrix2d expected_value =
            second_derivative_function(evaluated_variable);

        constexpr double tol = 0.05;
        CHECK_THAT(evaluated_value, eigen_approx(expected_value, tol));
    }

    SECTION("evaluate an operator for a two-dimensional variable with CSRBF") {
        using rbf_interpolator_type =
            local_csrbf_interpolator<double(Eigen::Vector2d)>;

        const auto function = [](const Eigen::Vector2d& variable) {
            return variable.squaredNorm();
        };
        const auto second_derivative_function =
            [](const Eigen::Vector2d& /*variable*/) {
                // NOLINTNEXTLINE(*-magic-numbers)
                return Eigen::Matrix2d{{2.0, 0.0}, {0.0, 2.0}};
            };

        const auto sample_variables = generate_halton_nodes<double, 2>(100);
        Eigen::VectorXd sample_values{};
        sample_values.resize(static_cast<index_type>(sample_variables.size()));
        for (std::size_t i = 0; i < sample_variables.size(); ++i) {
            sample_values(static_cast<index_type>(i)) =
                function(sample_variables[i]);
        }

        rbf_interpolator_type interpolator;
        constexpr double length_parameter_scale = 10.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::Vector2d evaluated_variable(0.3, 0.4);
        const Eigen::Matrix2d evaluated_value =
            interpolator.evaluate(hessian_operator(evaluated_variable));
        const Eigen::Matrix2d expected_value =
            second_derivative_function(evaluated_variable);

        constexpr double tol = 0.1;
        CHECK_THAT(evaluated_value, eigen_approx(expected_value, tol));
    }
}
