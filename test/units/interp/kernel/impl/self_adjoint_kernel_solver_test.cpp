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
 * \brief Test of self_adjoint_kernel_solver class.
 */
#include "num_collect/interp/kernel/impl/self_adjoint_kernel_solver.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "is_finite.h"
#include "num_collect/interp/kernel/calc_kernel_mat.h"
#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"
#include "num_collect/interp/kernel/rbf_kernel.h"

TEST_CASE("num_collect::interp::kernel::self_adjoint_kernel_solver") {
    using num_collect::interp::kernel::calc_kernel_mat;
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::rbf_kernel;
    using num_collect::interp::kernel::impl::self_adjoint_kernel_solver;

    const auto vars = std::vector<double>{0.0, 0.1, 0.2, 0.4, 0.6, 1.0};
    const auto data = Eigen::VectorXd{{0.0, 0.2, 0.4, 0.7, 1.0, 2.0}};

    auto kernel =
        rbf_kernel<euclidean_distance<double>, gaussian_rbf<double>>();
    constexpr double len_param = 0.1;
    kernel.len_param(len_param);

    const Eigen::MatrixXd kernel_mat = calc_kernel_mat(kernel, vars);

    SECTION("compute") {
        auto solver =
            self_adjoint_kernel_solver<Eigen::MatrixXd, Eigen::VectorXd>();
        solver.compute(kernel_mat, data);

        REQUIRE(solver.eigenvalues().size() == data.size());
        for (num_collect::index_type i = 0; i < data.size(); ++i) {
            INFO("i = " << i);
            REQUIRE(solver.eigenvalues()(i) > 0.0);
        }
    }

    SECTION("solve without regularization") {
        auto solver =
            self_adjoint_kernel_solver<Eigen::MatrixXd, Eigen::VectorXd>();
        solver.compute(kernel_mat, data);
        Eigen::VectorXd coeff;
        solver.solve(0.0, coeff);

        const Eigen::VectorXd retrieved_data = kernel_mat * coeff;
        REQUIRE_THAT(retrieved_data, eigen_approx(data));
    }

    SECTION("solve with regularization") {
        constexpr double reg_param = 1.234;
        auto solver =
            self_adjoint_kernel_solver<Eigen::MatrixXd, Eigen::VectorXd>();
        solver.compute(kernel_mat, data);
        Eigen::VectorXd coeff;
        solver.solve(reg_param, coeff);

        const Eigen::VectorXd retrieved_data =
            (kernel_mat +
                Eigen::MatrixXd::Identity(data.size(), data.size()) *
                    reg_param) *
            coeff;
        REQUIRE_THAT(retrieved_data, eigen_approx(data));
    }

    SECTION("calculate MLE objective function") {
        auto solver =
            self_adjoint_kernel_solver<Eigen::MatrixXd, Eigen::VectorXd>();
        solver.compute(kernel_mat, data);

        const double mle_zero = solver.calc_mle_objective(0.0);
        REQUIRE_THAT(mle_zero, is_finite());

        constexpr double large_param = 1e+3;
        const double mle_large = solver.calc_mle_objective(large_param);
        REQUIRE_THAT(mle_large, is_finite());

        REQUIRE(mle_large > mle_zero);
    }

    SECTION("calculate the coefficient of the kernel common in variables") {
        auto solver =
            self_adjoint_kernel_solver<Eigen::MatrixXd, Eigen::VectorXd>();
        solver.compute(kernel_mat, data);

        REQUIRE(solver.calc_common_coeff(0.0) > 0.0);
        constexpr double reg_param = 1e-3;
        REQUIRE(solver.calc_common_coeff(reg_param) > 0.0);
    }
}
