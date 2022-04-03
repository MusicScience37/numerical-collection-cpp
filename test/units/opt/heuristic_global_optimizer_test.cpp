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
 * \brief Test of heuristic_global_optimizer class.
 */
#include "num_collect/opt/heuristic_global_optimizer.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "eigen_approx.h"
#include "num_collect/base/index_type.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"
#include "num_prob_collect/opt/quadratic_function.h"

TEST_CASE("num_collect::opt::heuristic_global_optimizer(single_variate)") {
    using num_collect::opt::heuristic_global_optimizer;
    using num_prob_collect::opt::quadratic_function;

    SECTION("solve") {
        auto opt = heuristic_global_optimizer<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.init(left, right);
        REQUIRE_NOTHROW(opt.solve());

        constexpr double tol = 1e-2;
        REQUIRE_THAT(opt.opt_variable(), Catch::Matchers::WithinAbs(0.0, tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, tol));
    }
}

TEST_CASE("num_collect::opt::heuristic_global_optimizer(multi_variate)") {
    using num_collect::opt::heuristic_global_optimizer;
    using num_prob_collect::opt::multi_quadratic_function;

    SECTION("solve") {
        auto opt = heuristic_global_optimizer<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        constexpr num_collect::index_type opt1_max_evaluations = 200;
        constexpr double sol_tol = 1e-4;
        constexpr double tol_size = 0.1 * sol_tol;
        opt.opt1_max_evaluations(opt1_max_evaluations);
        opt.opt2_tol_simplex_size(tol_size);
        REQUIRE_NOTHROW(opt.solve());

        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Zero(3), sol_tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, sol_tol));
    }
}
