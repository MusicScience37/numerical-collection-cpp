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
 * \brief Test of sampling_optimizer class.
 */
#include "num_collect/opt/sampling_optimizer.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_prob_collect/opt/quadratic_function.h"

TEST_CASE("num_collect::opt::sampling_optimizer") {
    using num_collect::opt::sampling_optimizer;
    using num_prob_collect::opt::quadratic_function;

    constexpr num_collect::index_type num_samples = 11;

    SECTION("init") {
        auto opt = sampling_optimizer<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.num_samples(num_samples);
        opt.init(left, right);
        REQUIRE(opt.upper() < right);
        REQUIRE(opt.lower() > left);
        REQUIRE(opt.iterations() == 1);
        REQUIRE(opt.evaluations() == num_samples);
    }

    SECTION("iterate") {
        auto opt = sampling_optimizer<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.num_samples(num_samples);
        opt.init(left, right);
        opt.iterate();
        REQUIRE(opt.upper() < right);
        REQUIRE(opt.lower() > left);
        REQUIRE(opt.iterations() == 2);
        REQUIRE(opt.evaluations() == 2 * num_samples);
    }

    SECTION("iterate twice") {
        auto opt = sampling_optimizer<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.num_samples(num_samples);
        opt.init(left, right);
        opt.iterate();
        opt.iterate();
        REQUIRE(opt.upper() < right);
        REQUIRE(opt.lower() > left);
        REQUIRE(opt.iterations() == 3);
        REQUIRE(opt.evaluations() == 3 * num_samples);
    }

    SECTION("solve") {
        auto opt = sampling_optimizer<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.num_samples(num_samples);
        opt.init(left, right);
        REQUIRE_NOTHROW(opt.solve());
        constexpr double tol = 1e-2;
        REQUIRE_THAT(opt.opt_variable(), Catch::Matchers::WithinAbs(0.0, tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, tol));
    }
}
