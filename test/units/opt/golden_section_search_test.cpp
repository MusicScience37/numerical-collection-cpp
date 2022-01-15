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
 * \brief Test of golden_section_search class.
 */
#include "num_collect/opt/golden_section_search.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_prob_collect/opt/quadratic_function.h"

TEST_CASE("num_collect::opt::golden_section_search") {
    using num_collect::opt::golden_section_search;
    using num_prob_collect::opt::quadratic_function;

    SECTION("init") {
        auto opt = golden_section_search<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.init(left, right);
        REQUIRE_THAT(
            opt.section_len(), Catch::Matchers::WithinRel(right - left));
        REQUIRE(opt.iterations() == 0);
        REQUIRE(opt.evaluations() == 1);
    }

    SECTION("iterate") {
        auto opt = golden_section_search<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.init(left, right);
        opt.iterate();
        constexpr double coeff = 0.618033988749895;
        constexpr double section_len = (right - left) * coeff;
        REQUIRE_THAT(
            opt.section_len(), Catch::Matchers::WithinRel(section_len));
        REQUIRE(opt.iterations() == 1);
        REQUIRE(opt.evaluations() == 2);
    }

    SECTION("iterate twice") {
        auto opt = golden_section_search<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.init(left, right);
        opt.iterate();
        opt.iterate();
        constexpr double coeff = 0.618033988749895;
        constexpr double section_len = (right - left) * coeff * coeff;
        REQUIRE_THAT(
            opt.section_len(), Catch::Matchers::WithinRel(section_len));
        REQUIRE(opt.iterations() == 2);
        REQUIRE(opt.evaluations() == 3);
    }

    SECTION("solve") {
        auto opt = golden_section_search<quadratic_function>();
        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.init(left, right);
        constexpr double tol = 1e-3;
        opt.tol_section_len(tol);
        opt.solve();
        REQUIRE(opt.section_len() < tol);
        REQUIRE_THAT(opt.opt_variable(), Catch::Matchers::WithinAbs(0.0, tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, tol));
    }
}
