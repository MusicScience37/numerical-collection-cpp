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
 * \brief Test of backtracking_line_searcher class.
 */
#include "num_collect/opt/backtracking_line_searcher.h"

#include <catch2/catch_test_macros.hpp>

#include "eigen_approx.h"
#include "num_collect/opt/concepts/line_searcher.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"

TEST_CASE("num_collect::opt::backtracking_line_searcher") {
    using num_collect::opt::backtracking_line_searcher;
    using num_prob_collect::opt::multi_quadratic_function;

    auto searcher = backtracking_line_searcher<multi_quadratic_function>();

    SECTION("concepts") {
        STATIC_REQUIRE(num_collect::opt::concepts::line_searcher<
            backtracking_line_searcher<multi_quadratic_function>>);
    }

    SECTION("init") {
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        searcher.init(init_var);
        REQUIRE(searcher.evaluations() == 1);
        REQUIRE_THAT(searcher.opt_variable(), eigen_approx(init_var));
    }

    SECTION("search") {
        const Eigen::VectorXd init_var =
            (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
        searcher.init(init_var);
        const auto prev_value = searcher.opt_value();

        REQUIRE_NOTHROW(searcher.search(-searcher.gradient()));
        REQUIRE(searcher.last_step() < 1.0);
        REQUIRE(searcher.last_step() > 0.0);
        REQUIRE(searcher.evaluations() > 2);
        REQUIRE(searcher.opt_value() < prev_value);
    }
}
