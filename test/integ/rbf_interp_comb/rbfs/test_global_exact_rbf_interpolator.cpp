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
 * \brief Test of different RBFs in RBF interpolation.
 */
#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/constants/pi.h"
#include "num_collect/rbf/global_exact_rbf_interpolator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/rbf/rbfs/inverse_multi_quadric_rbf.h"
#include "num_collect/rbf/rbfs/inverse_quadratic_rbf.h"

TEMPLATE_TEST_CASE("global_exact_rbf_interpolator with different RBFs", "",
    num_collect::rbf::rbfs::gaussian_rbf<double>,
    // num_collect::rbf::rbfs::multi_quadric_rbf<double> didn't work.
    num_collect::rbf::rbfs::inverse_multi_quadric_rbf<double>,
    num_collect::rbf::rbfs::inverse_quadratic_rbf<double>) {
    using num_collect::rbf::global_exact_rbf_interpolator;

    using variable_type = double;
    using rbf_type = TestType;
    using rbf_interpolator_type =
        global_exact_rbf_interpolator<variable_type, variable_type, rbf_type>;

    rbf_interpolator_type interpolator;

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

    SECTION("interpolate with a fixed scale") {
        constexpr double length_parameter_scale = 2.0;
        interpolator.fix_length_parameter_scale(length_parameter_scale);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::VectorXd interpolated_variables =
            Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
        Eigen::VectorXd interpolated_values;
        interpolated_values.resize(interpolated_variables.size());
        Eigen::VectorXd actual_values;
        actual_values.resize(interpolated_variables.size());
        Eigen::VectorXd variances;
        variances.resize(interpolated_variables.size());
        for (num_collect::index_type i = 0; i < interpolated_variables.size();
             ++i) {
            std::tie(interpolated_values(i), variances(i)) =
                interpolator.evaluate_mean_and_variance_on(
                    interpolated_variables(i), sample_variables);
            actual_values(i) = function(interpolated_variables(i));
        }
        const Eigen::VectorXd standard_deviations = variances.cwiseSqrt();
        comparison_approvals::verify_with_reference_and_error(
            actual_values, standard_deviations, interpolated_values, 2);
    }

    SECTION("interpolate with a optimal scale") {
        interpolator.optimize_length_parameter_scale(
            sample_variables, sample_values);
        interpolator.compute(sample_variables, sample_values);

        const Eigen::VectorXd interpolated_variables =
            Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);
        Eigen::VectorXd interpolated_values;
        interpolated_values.resize(interpolated_variables.size());
        Eigen::VectorXd actual_values;
        actual_values.resize(interpolated_variables.size());
        Eigen::VectorXd variances;
        variances.resize(interpolated_variables.size());
        for (num_collect::index_type i = 0; i < interpolated_variables.size();
             ++i) {
            std::tie(interpolated_values(i), variances(i)) =
                interpolator.evaluate_mean_and_variance_on(
                    interpolated_variables(i), sample_variables);
            actual_values(i) = function(interpolated_variables(i));
        }
        const Eigen::VectorXd standard_deviations = variances.cwiseSqrt();
        comparison_approvals::verify_with_reference_and_error(
            actual_values, standard_deviations, interpolated_values, 3);
    }
}
