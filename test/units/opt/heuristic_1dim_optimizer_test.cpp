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
 * \brief Test of heuristic_1dim_optimizer class.
 */
#include "num_collect/opt/heuristic_1dim_optimizer.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_prob_collect/opt/quadratic_function.h"

TEST_CASE("num_collect::opt::heuristic_1dim_optimizer") {
    using num_collect::opt::heuristic_1dim_optimizer;
    using num_prob_collect::opt::quadratic_function;

    SECTION("solve") {
        auto opt = heuristic_1dim_optimizer<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.init(left, right);
        REQUIRE_NOTHROW(opt.solve());
        constexpr double tol = 1e-2;
        REQUIRE_THAT(opt.opt_variable(), Catch::Matchers::WithinAbs(0.0, tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, tol));
    }
}
