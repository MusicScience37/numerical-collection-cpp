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
 * \brief Test of compute_kernel_matrix_parallel function.
 */
#include "num_collect/rbf/impl/compute_kernel_matrix_parallel.h"

#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "fmt_approval_tests.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/length_parameter_calculators/local_length_parameter_calculator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"
#include "num_collect/util/format_dense_matrix.h"
#include "num_collect/util/format_sparse_matrix.h"

TEST_CASE("num_collect::rbf::impl::compute_kernel_matrix_parallel") {
    using num_collect::rbf::distance_functions::euclidean_distance_function;
    using num_collect::rbf::impl::compute_kernel_matrix_parallel;
    using num_collect::rbf::length_parameter_calculators::
        global_length_parameter_calculator;
    using num_collect::rbf::length_parameter_calculators::
        local_length_parameter_calculator;
    using num_collect::rbf::rbfs::gaussian_rbf;
    using num_collect::rbf::rbfs::wendland_csrbf;

    SECTION("compute a matrix with global length parameters") {
        using variable_type = double;
        using scalar_type = double;
        using distance_function_type =
            euclidean_distance_function<variable_type>;
        using rbf_type = gaussian_rbf<scalar_type>;
        using length_parameter_calculator_type =
            global_length_parameter_calculator<distance_function_type>;

        const distance_function_type distance_function;
        const rbf_type rbf;
        length_parameter_calculator_type length_parameter_calculator;
        const auto variables = std::vector<double>{0.0, 0.3, 0.5, 0.6};
        Eigen::MatrixXd kernel_matrix;

        compute_kernel_matrix_parallel(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);

        ApprovalTests::Approvals::verify(fmt::format("{: 10.3e}",
            num_collect::util::format_dense_matrix(kernel_matrix,
                num_collect::util::dense_matrix_format_type::multi_line)));
    }

    SECTION("compute a matrix with local length parameters") {
        using variable_type = double;
        using scalar_type = double;
        using distance_function_type =
            euclidean_distance_function<variable_type>;
        using rbf_type = gaussian_rbf<scalar_type>;
        using length_parameter_calculator_type =
            local_length_parameter_calculator<distance_function_type>;

        const distance_function_type distance_function;
        const rbf_type rbf;
        length_parameter_calculator_type length_parameter_calculator;
        const auto variables = std::vector<double>{0.0, 0.3, 0.5, 0.6};
        Eigen::MatrixXd kernel_matrix;

        compute_kernel_matrix_parallel(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);

        ApprovalTests::Approvals::verify(fmt::format("{: 10.3e}",
            num_collect::util::format_dense_matrix(kernel_matrix,
                num_collect::util::dense_matrix_format_type::multi_line)));
    }

    SECTION("compute a sparse matrix with local length parameter") {
        using variable_type = double;
        using scalar_type = double;
        using distance_function_type =
            euclidean_distance_function<variable_type>;
        using rbf_type = wendland_csrbf<scalar_type, 1, 1>;
        using length_parameter_calculator_type =
            local_length_parameter_calculator<distance_function_type>;

        const distance_function_type distance_function;
        const rbf_type rbf;
        length_parameter_calculator_type length_parameter_calculator;
        length_parameter_calculator.scale(2.0);
        const auto variables = std::vector<double>{0.0, 0.3, 0.5, 0.6};
        Eigen::SparseMatrix<double> kernel_matrix;

        compute_kernel_matrix_parallel(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);

        ApprovalTests::Approvals::verify(fmt::format("{: 10.3e}",
            num_collect::util::format_sparse_matrix(kernel_matrix,
                num_collect::util::sparse_matrix_format_type::multi_line)));
    }
}
