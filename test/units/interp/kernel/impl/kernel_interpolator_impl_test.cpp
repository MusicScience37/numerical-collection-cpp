/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of kernel_interpolator_impl class.
 */
#include "num_collect/interp/kernel/impl/kernel_interpolator_impl.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "is_finite.h"
#include "num_collect/interp/kernel/calc_kernel_mat.h"
#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"
#include "num_collect/interp/kernel/rbf_kernel.h"

TEST_CASE("num_collect::interp::kernel::impl::kernel_interpolator_impl") {
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::rbf_kernel;
    using num_collect::interp::kernel::impl::kernel_interpolator_impl;

    const auto vars = std::vector<double>{0.0, 0.1, 0.2, 0.4, 0.6, 1.0};
    const auto data = Eigen::VectorXd{{0.0, 0.2, 0.4, 0.7, 1.0, 2.0}};

    constexpr double len_param = 0.1;

    const auto kernel =
        rbf_kernel<euclidean_distance<double>, gaussian_rbf<double>>()
            .len_param(len_param);

    const Eigen::MatrixXd kernel_mat = calc_kernel_mat(kernel, vars);

    SECTION("compute with default settings") {
        auto interpolator = kernel_interpolator_impl<double>();

        REQUIRE(interpolator.reg_param() == 0.0);  // NOLINT

        interpolator.compute(kernel, vars, data);

        REQUIRE(interpolator.reg_param() == 0.0);  // NOLINT
        REQUIRE_THAT(interpolator.mle_objective_function_value(), is_finite());

        Eigen::VectorXd coeff;
        interpolator.calc_coeff(coeff);
        const Eigen::VectorXd retrieved_data = kernel_mat * coeff;
        constexpr double tol_error = 1e-12;
        REQUIRE_THAT(retrieved_data, eigen_approx(data, tol_error));
    }

    SECTION("compute with a fixed regularization parameter") {
        auto interpolator = kernel_interpolator_impl<double>();

        constexpr double reg_param = 1e-4;
        interpolator.regularize_with(reg_param);
        REQUIRE(interpolator.reg_param() == reg_param);

        interpolator.compute(kernel, vars, data);

        REQUIRE(interpolator.reg_param() == reg_param);
        REQUIRE_THAT(interpolator.mle_objective_function_value(), is_finite());

        Eigen::VectorXd coeff;
        interpolator.calc_coeff(coeff);
        const Eigen::VectorXd retrieved_data = kernel_mat * coeff;
        constexpr double tol_error = 1e-2;
        REQUIRE_THAT(retrieved_data, eigen_approx(data, tol_error));
    }

    SECTION("compute with automatic regularization") {
        auto interpolator = kernel_interpolator_impl<double>();

        interpolator.regularize_automatically();
        interpolator.compute(kernel, vars, data);

        REQUIRE(interpolator.reg_param() > 0.0);
        REQUIRE_THAT(interpolator.mle_objective_function_value(), is_finite());

        Eigen::VectorXd coeff;
        interpolator.calc_coeff(coeff);
        const Eigen::VectorXd retrieved_data = kernel_mat * coeff;
        constexpr double tol_error = 1e-4;
        REQUIRE_THAT(retrieved_data, eigen_approx(data, tol_error));
    }
}
