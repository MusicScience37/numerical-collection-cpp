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
 * \brief Test of general_spline_equation_solver class.
 */
#include "num_collect/rbf/impl/general_spline_equation_solver.h"

#include <cstddef>
#include <vector>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/constants/pi.h"
#include "num_collect/rbf/compute_kernel_matrix.h"
#include "num_collect/rbf/compute_polynomial_term_matrix.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/kernel_matrix_type.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/polynomial_term_generator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

TEST_CASE(
    "num_collect::rbf::impl::general_spline_equation_solver "
    "(for dense kernel matrix and global length parameter)") {
    using num_collect::rbf::compute_kernel_matrix;
    using num_collect::rbf::compute_polynomial_term_matrix;
    using num_collect::rbf::kernel_matrix_type;
    using num_collect::rbf::polynomial_term_generator;
    using num_collect::rbf::distance_functions::euclidean_distance_function;
    using num_collect::rbf::impl::general_spline_equation_solver;
    using num_collect::rbf::length_parameter_calculators::
        global_length_parameter_calculator;
    using num_collect::rbf::rbfs::gaussian_rbf;

    using variable_type = double;
    using value_type = double;
    using distance_function_type = euclidean_distance_function<variable_type>;
    using rbf_type = gaussian_rbf<value_type>;
    using length_parameter_calculator_type =
        global_length_parameter_calculator<distance_function_type>;
    using kernel_matrix_solver_type = general_spline_equation_solver<double,
        double, kernel_matrix_type::dense, true>;

    kernel_matrix_solver_type solver;

    const auto function = [](double x) {
        return std::cos(num_collect::constants::pi<double> * x);
    };

    SECTION("compute internal parameters") {
        const auto sample_variables = std::vector<double>{0.0, 0.5, 0.8, 1.0};
        Eigen::VectorXd sample_values{};
        sample_values.resize(
            static_cast<num_collect::index_type>(sample_variables.size()));
        for (std::size_t i = 0; i < sample_variables.size(); ++i) {
            sample_values(static_cast<num_collect::index_type>(i)) =
                function(sample_variables[i]);
        }

        const distance_function_type distance_function;
        const rbf_type rbf;
        length_parameter_calculator_type length_parameter_calculator;
        Eigen::MatrixXd kernel_matrix;
        compute_kernel_matrix(distance_function, rbf,
            length_parameter_calculator, sample_variables, kernel_matrix);
        Eigen::MatrixXd additional_matrix;
        const polynomial_term_generator<1> generator(1);
        compute_polynomial_term_matrix(
            sample_variables, additional_matrix, generator);

        REQUIRE_NOTHROW(
            solver.compute(kernel_matrix, additional_matrix, sample_values));

        SECTION("solve for a regularization parameter") {
            constexpr double reg_param = 1e-4;

            Eigen::VectorXd kernel_coeffs;
            Eigen::VectorXd additional_coeffs;
            REQUIRE_NOTHROW(
                solver.solve(kernel_coeffs, additional_coeffs, reg_param));

            CHECK(kernel_coeffs.rows() == 4);
            CHECK(kernel_coeffs.allFinite());
            CHECK(additional_coeffs.rows() == 2);
            CHECK(additional_coeffs.allFinite());
            // Values will be checked in tests of RBF interpolation.
        }

        SECTION("calculate the objective function of MLE") {
            constexpr double reg_param = 1e-4;

            (void)solver.calc_mle_objective(reg_param);
            // Result will be checked in tests of optimization of this value.
        }
    }
}
