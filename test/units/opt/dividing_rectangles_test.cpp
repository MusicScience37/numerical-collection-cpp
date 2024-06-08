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
 * \brief Test of dividing_rectangles class.
 */
#include "num_collect/opt/dividing_rectangles.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/opt/concepts/box_constrained_optimizer.h"
#include "num_collect/opt/concepts/optimizer.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"
#include "num_prob_collect/opt/vibrated_quadratic_function.h"

TEST_CASE("num_collect::opt::dividing_rectangles (multi variables)") {
    using num_collect::opt::dividing_rectangles;
    using num_prob_collect::opt::multi_quadratic_function;

    SECTION("concepts") {
        STATIC_REQUIRE(num_collect::opt::concepts::optimizer<
            dividing_rectangles<multi_quadratic_function>>);
        STATIC_REQUIRE(num_collect::opt::concepts::box_constrained_optimizer<
            dividing_rectangles<multi_quadratic_function>>);
    }

    SECTION("init") {
        auto opt = dividing_rectangles<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        REQUIRE(opt.iterations() == 0);
        REQUIRE(opt.evaluations() == 1);  // NOLINT
        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Constant(3, 0.5)));  // NOLINT
    }

    SECTION("iterate") {
        auto opt = dividing_rectangles<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        const auto prev_value = opt.opt_value();

        opt.iterate();
        REQUIRE(opt.iterations() == 1);
        REQUIRE(opt.evaluations() == 7);
        REQUIRE(opt.opt_value() <= prev_value);
    }

    SECTION("solve") {
        auto opt = dividing_rectangles<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        constexpr double sol_tol = 1e-2;
        opt.max_evaluations(1000);  // NOLINT
        opt.solve();
        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Zero(3), sol_tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, sol_tol));
    }
}

TEST_CASE("num_collect::opt::dividing_rectangles (one variable)") {
    using num_collect::opt::dividing_rectangles;
    using num_prob_collect::opt::vibrated_quadratic_function;

    SECTION("concepts") {
        STATIC_REQUIRE(num_collect::opt::concepts::optimizer<
            dividing_rectangles<vibrated_quadratic_function>>);
        STATIC_REQUIRE(num_collect::opt::concepts::box_constrained_optimizer<
            dividing_rectangles<vibrated_quadratic_function>>);
    }

    SECTION("init") {
        auto opt = dividing_rectangles<vibrated_quadratic_function>();
        opt.init(-5.0, 10.0);  // NOLINT
        REQUIRE(opt.iterations() == 0);
        REQUIRE(opt.evaluations() == 1);  // NOLINT
        REQUIRE_THAT(
            opt.opt_variable(), Catch::Matchers::WithinRel(2.5));  // NOLINT
    }

    SECTION("iterate") {
        auto opt = dividing_rectangles<vibrated_quadratic_function>();
        opt.init(-5.0, 10.0);  // NOLINT
        const auto prev_value = opt.opt_value();

        opt.iterate();
        REQUIRE(opt.iterations() == 1);
        REQUIRE(opt.evaluations() == 3);
        REQUIRE(opt.opt_value() <= prev_value);
    }

    SECTION("solve") {
        auto opt = dividing_rectangles<vibrated_quadratic_function>();
        opt.init(-5.0, 10.0);  // NOLINT
        constexpr double sol_tol = 1e-2;
        opt.max_evaluations(1000);  // NOLINT
        opt.solve();
        REQUIRE_THAT(opt.opt_variable(),
            Catch::Matchers::WithinAbs(0.0, sol_tol));  // NOLINT
        REQUIRE_THAT(
            opt.opt_value(), Catch::Matchers::WithinAbs(-1.0, sol_tol));
    }
}
