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
 * \brief Test of conjugate_gradient_optimizer class.
 */
#include "num_collect/opt/conjugate_gradient_optimizer.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/opt/concepts/descent_method.h"
#include "num_collect/opt/concepts/optimizer.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"

TEST_CASE("num_collect::opt::conjugate_gradient_optimizer") {
    using num_collect::opt::conjugate_gradient_optimizer;
    using num_prob_collect::opt::multi_quadratic_function;

    SECTION("concepts") {
        STATIC_REQUIRE(num_collect::opt::concepts::optimizer<
            conjugate_gradient_optimizer<multi_quadratic_function>>);
        STATIC_REQUIRE(num_collect::opt::concepts::descent_method<
            conjugate_gradient_optimizer<multi_quadratic_function>>);
    }

    SECTION("init") {
        auto opt = conjugate_gradient_optimizer<multi_quadratic_function>();
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        opt.init(init_var);
        REQUIRE(opt.iterations() == 0);
        REQUIRE(opt.evaluations() == 1);
        REQUIRE_THAT(opt.opt_variable(), eigen_approx(init_var));
    }

    SECTION("iterate") {
        auto opt = conjugate_gradient_optimizer<multi_quadratic_function>();
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        opt.init(init_var);
        const auto prev_value = opt.opt_value();

        opt.iterate();
        REQUIRE(opt.iterations() == 1);
        REQUIRE(opt.evaluations() >= 2);
        REQUIRE(opt.opt_value() < prev_value);
    }

    SECTION("solve") {
        auto opt = conjugate_gradient_optimizer<multi_quadratic_function>();
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        opt.init(init_var);
        constexpr double tol = 1e-3;
        opt.tol_gradient_norm(tol);
        opt.solve();
        REQUIRE(opt.gradient_norm() < tol);
        REQUIRE_THAT(
            opt.opt_variable(), eigen_approx(Eigen::VectorXd::Zero(3), tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, tol));
    }
}
