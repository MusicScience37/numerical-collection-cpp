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
 * \brief Test of different dimensions in RBF interpolation.
 */
#include <cstddef>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/impl/get_variable_type.h"
#include "num_collect/rbf/rbf_interpolator.h"

TEMPLATE_TEST_CASE_SIG("local_csrbf_interpolator with dimensions", "",
    ((num_collect::index_type Dimension), Dimension), (2), (3), (4), (5), (6)) {
    using num_collect::rbf::generate_halton_nodes;
    using num_collect::rbf::local_csrbf_interpolator;

    constexpr num_collect::index_type dimensions = Dimension;
    using variable_type = Eigen::Vector<double, dimensions>;
    using rbf_interpolator_type =
        local_csrbf_interpolator<double(variable_type)>;

    rbf_interpolator_type interpolator;

    const auto function = [](const variable_type& var) {
        return var.squaredNorm();
    };

    const auto sample_variables =
        generate_halton_nodes<double, dimensions>(100);
    Eigen::VectorXd sample_values{};
    sample_values.resize(
        static_cast<num_collect::index_type>(sample_variables.size()));
    for (std::size_t i = 0; i < sample_variables.size(); ++i) {
        sample_values(static_cast<num_collect::index_type>(i)) =
            function(sample_variables[i]);
    }

    interpolator.compute(sample_variables, sample_values);

    const Eigen::VectorXd interpolated_variable_elements =
        Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
    Eigen::VectorXd interpolated_values;
    interpolated_values.resize(interpolated_variable_elements.size());
    Eigen::VectorXd actual_values;
    actual_values.resize(interpolated_variable_elements.size());
    for (num_collect::index_type i = 0;
         i < interpolated_variable_elements.size(); ++i) {
        const variable_type interpolated_variable =
            variable_type::Constant(interpolated_variable_elements(i));
        interpolated_values(i) =
            interpolator.interpolate(interpolated_variable);
        actual_values(i) = function(interpolated_variable);
    }
    comparison_approvals::verify_with_reference(
        interpolated_values, actual_values, 2);
}
