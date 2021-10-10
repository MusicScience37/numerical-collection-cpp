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
 * \brief Test of kernel_parameter_optimizer class.
 */
#include "num_collect/interp/kernel/impl/kernel_parameter_optimizer.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "is_finite.h"
#include "num_collect/interp/kernel/calc_kernel_mat.h"
#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"
#include "num_collect/interp/kernel/impl/auto_regularizer_wrapper.h"
#include "num_collect/interp/kernel/rbf_kernel.h"

TEST_CASE("num_collect::interp::kernel::impl::kernel_parameter_optimizer") {
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::rbf_kernel;
    using num_collect::interp::kernel::impl::auto_regularizer_wrapper;
    using num_collect::interp::kernel::impl::kernel_parameter_optimizer;

    const auto vars = std::vector<double>{0.0, 0.1, 0.2, 0.4, 0.6, 1.0};
    const auto data = Eigen::VectorXd{{0.0, 0.2, 0.4, 0.7, 1.0, 2.0}};

    using kernel_type =
        rbf_kernel<euclidean_distance<double>, gaussian_rbf<double>>;
    auto kernel = kernel_type();

    auto interpolator = auto_regularizer_wrapper<double>();

    SECTION("compute") {
        auto optimizer =
            kernel_parameter_optimizer<kernel_type>(interpolator, kernel);

        optimizer.compute(vars, data);

        kernel.kernel_param(optimizer.opt_param());
        const Eigen::MatrixXd kernel_mat = calc_kernel_mat(kernel, vars);

        interpolator.compute(kernel, vars, data);
        Eigen::VectorXd coeff;
        interpolator.solve(coeff);

        const Eigen::VectorXd retrieved_data = kernel_mat * coeff;
        constexpr double tol_error = 1e-4;
        REQUIRE_THAT(retrieved_data, eigen_approx(data, tol_error));
    }
}
