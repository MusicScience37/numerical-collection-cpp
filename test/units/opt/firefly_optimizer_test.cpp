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
 * \brief Test of firefly_optimizer class.
 */
#include "num_collect/opt/firefly_optimizer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/opt/concepts/box_constrained_optimizer.h"
#include "num_collect/opt/concepts/optimizer.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"

TEST_CASE("num_collect::opt::firefly_optimizer (multi variables)") {
    using num_collect::opt::firefly_optimizer;
    using num_prob_collect::opt::multi_quadratic_function;

    SECTION("concepts") {
        STATIC_REQUIRE(num_collect::opt::concepts::optimizer<
            firefly_optimizer<multi_quadratic_function>>);
        STATIC_REQUIRE(num_collect::opt::concepts::box_constrained_optimizer<
            firefly_optimizer<multi_quadratic_function>>);
    }

    SECTION("init") {
        auto opt = firefly_optimizer<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -1.0),
            Eigen::VectorXd::Constant(3, 2.0));
        CHECK(opt.iterations() == 0);
        CHECK(opt.evaluations() == 200);
    }

    SECTION("iterate") {
        auto opt = firefly_optimizer<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -1.0),
            Eigen::VectorXd::Constant(3, 2.0));
        const auto prev_value = opt.opt_value();

        opt.iterate();
        CHECK(opt.iterations() == 1);
        CHECK(opt.evaluations() == 400);
        CHECK(opt.opt_value() <= prev_value);
    }

    SECTION("solve") {
        auto opt = firefly_optimizer<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -1.0),
            Eigen::VectorXd::Constant(3, 2.0));
        constexpr double sol_tol = 1e-1;
        opt.solve();
        CHECK_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Zero(3), sol_tol));
        CHECK_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, sol_tol));
    }
}
