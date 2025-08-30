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
 * \brief Test of different RBFs in evaluation of fourth-order derivatives of
 * RBF interpolation.
 */
#include <cmath>
#include <cstddef>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/biharmonic_operator.h"
#include "num_collect/rbf/rbf_interpolator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

TEMPLATE_TEST_CASE(
    "fourth-order derivative of local_rbf_interpolator with different RBFs", "",
    num_collect::rbf::rbfs::gaussian_rbf<double>) {
    using num_collect::rbf::generate_1d_halton_nodes;
    using num_collect::rbf::local_rbf_interpolator;
    using num_collect::rbf::operators::biharmonic_operator;

    using variable_type = double;
    using rbf_type = TestType;
    using rbf_interpolator_type =
        local_rbf_interpolator<variable_type(variable_type), rbf_type>;

    rbf_interpolator_type interpolator;

    const auto function = [](double x) {
        return std::cos(num_collect::constants::pi<double> * x);
    };
    const auto fourth_derivative_function = [](double x) {
        return num_collect::constants::pi<double> *
            num_collect::constants::pi<double> *
            num_collect::constants::pi<double> *
            num_collect::constants::pi<double> *
            std::cos(num_collect::constants::pi<double> * x);
    };

    const auto sample_variables = generate_1d_halton_nodes<double>(10);
    Eigen::VectorXd sample_values{};
    sample_values.resize(
        static_cast<num_collect::index_type>(sample_variables.size()));
    for (std::size_t i = 0; i < sample_variables.size(); ++i) {
        sample_values(static_cast<num_collect::index_type>(i)) =
            function(sample_variables[i]);
    }

    interpolator.compute(sample_variables, sample_values);

    const Eigen::VectorXd interpolated_variables =
        Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
    Eigen::VectorXd interpolated_values;
    interpolated_values.resize(interpolated_variables.size());
    Eigen::VectorXd actual_values;
    actual_values.resize(interpolated_variables.size());
    for (num_collect::index_type i = 0; i < interpolated_variables.size();
        ++i) {
        interpolated_values(i) = interpolator.evaluate(
            biharmonic_operator(interpolated_variables(i)));
        actual_values(i) =
            fourth_derivative_function(interpolated_variables(i));
    }
    comparison_approvals::verify_with_reference(
        interpolated_values, actual_values, 2);
}
