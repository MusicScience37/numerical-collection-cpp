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
 * \brief Test of downhill_simplex class.
 */
#include "num_collect/opt/downhill_simplex.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/opt/concepts/optimizer.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"

TEST_CASE("num_collect::opt::downhill_simplex") {
    using num_collect::opt::downhill_simplex;
    using num_prob_collect::opt::multi_quadratic_function;

    SECTION("concepts") {
        STATIC_REQUIRE(num_collect::opt::concepts::optimizer<
            downhill_simplex<multi_quadratic_function>>);
    }

    SECTION("init") {
        auto opt = downhill_simplex<multi_quadratic_function>();
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        opt.init(init_var);
        REQUIRE(opt.iterations() == 0);
        REQUIRE(opt.evaluations() == 4);  // NOLINT
        REQUIRE(opt.last_process() == decltype(opt)::process_type::none);
        REQUIRE_THAT(opt.opt_variable(), eigen_approx(init_var));
    }

    SECTION("iterate") {
        auto opt = downhill_simplex<multi_quadratic_function>();
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        opt.init(init_var);
        const auto prev_value = opt.opt_value();

        opt.iterate();
        REQUIRE(opt.iterations() == 1);
        REQUIRE(opt.evaluations() >= 5);
        REQUIRE(opt.last_process() != decltype(opt)::process_type::none);
        REQUIRE(opt.opt_value() <= prev_value);
    }

    SECTION("solve") {
        auto opt = downhill_simplex<multi_quadratic_function>();
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        opt.init(init_var);
        constexpr double tol_size = 1e-4;
        constexpr double sol_tol = 10.0 * tol_size;
        opt.tol_simplex_size(tol_size);
        opt.solve();
        REQUIRE(opt.simplex_size() < tol_size);
        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Zero(3), sol_tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, sol_tol));
    }
}
