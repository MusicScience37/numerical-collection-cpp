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
 * \brief Test of auto_regularizer class.
 */
#include "num_collect/interp/kernel/impl/auto_regularizer.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "is_finite.h"
#include "num_collect/interp/kernel/calc_kernel_mat.h"
#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"
#include "num_collect/interp/kernel/impl/self_adjoint_kernel_solver.h"
#include "num_collect/interp/kernel/rbf_kernel.h"

TEST_CASE("num_collect::interp::kernel::impl::auto_regularizer") {
    using num_collect::interp::kernel::calc_kernel_mat;
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::rbf_kernel;
    using num_collect::interp::kernel::impl::auto_regularizer;
    using num_collect::interp::kernel::impl::self_adjoint_kernel_solver;

    const auto vars = std::vector<double>{0.0, 0.1, 0.2, 0.4, 0.6, 1.0};
    const auto data = Eigen::VectorXd{{0.0, 0.2, 0.4, 0.7, 1.0, 2.0}};

    auto kernel =
        rbf_kernel<euclidean_distance<double>, gaussian_rbf<double>>();
    constexpr double len_param = 0.1;
    kernel.len_param(len_param);

    const Eigen::MatrixXd kernel_mat = calc_kernel_mat(kernel, vars);

    using solver_type =
        self_adjoint_kernel_solver<Eigen::MatrixXd, Eigen::VectorXd>;
    auto solver = solver_type();
    solver.compute(kernel_mat, data);

    SECTION("optimize") {
        auto regularizer = auto_regularizer<solver_type>(solver);

        regularizer.optimize();

        REQUIRE(regularizer.opt_param() > 0.0);
        REQUIRE_THAT(regularizer.opt_value(), is_finite());

        Eigen::VectorXd coeff;
        solver.solve(regularizer.opt_param(), coeff);

        const Eigen::VectorXd retrieved_data = kernel_mat * coeff;
        constexpr double tol_error = 1e-3;
        REQUIRE_THAT(retrieved_data, eigen_approx(data, tol_error));
    }
}
