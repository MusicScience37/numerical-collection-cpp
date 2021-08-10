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
 * \brief Test of node class.
 */
#include "num_collect/auto_diff/backward/graph/node_differentiator.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::auto_diff::backward::graph::node_differentiator") {
    using num_collect::auto_diff::backward::graph::child_node;
    using num_collect::auto_diff::backward::graph::create_node;
    using num_collect::auto_diff::backward::graph::node;
    using num_collect::auto_diff::backward::graph::node_differentiator;

    SECTION("compute one node") {
        const auto n1 = std::make_shared<node<double>>();

        auto diff = node_differentiator<double>();
        REQUIRE_NOTHROW(diff.compute(n1));
        REQUIRE_THAT(diff.coeff(n1), Catch::Matchers::WithinRel(1.0));
    }

    SECTION("compute two nodes") {
        const auto n1 = create_node<double>();
        constexpr double c12 = 1.234;
        const auto n2 = create_node<double>(n1, c12);

        auto diff = node_differentiator<double>();
        REQUIRE_NOTHROW(diff.compute(n2));
        REQUIRE_THAT(diff.coeff(n1), Catch::Matchers::WithinRel(c12));
        REQUIRE_THAT(diff.coeff(n2), Catch::Matchers::WithinRel(1.0));
    }

    SECTION("compute three parallel nodes") {
        const auto n1 = create_node<double>();
        const auto n2 = create_node<double>();
        constexpr double c13 = 1.234;
        constexpr double c23 = 2.468;
        const auto n3 = create_node<double>(n1, c13, n2, c23);

        auto diff = node_differentiator<double>();
        REQUIRE_NOTHROW(diff.compute(n3));
        REQUIRE_THAT(diff.coeff(n1), Catch::Matchers::WithinRel(c13));
        REQUIRE_THAT(diff.coeff(n2), Catch::Matchers::WithinRel(c23));
    }

    SECTION("compute three series nodes") {
        const auto n1 = create_node<double>();
        constexpr double c12 = 1.234;
        const auto n2 = create_node<double>(n1, c12);
        constexpr double c23 = 2.468;
        const auto n3 = create_node<double>(n2, c23);

        auto diff = node_differentiator<double>();
        REQUIRE_NOTHROW(diff.compute(n3));
        REQUIRE_THAT(diff.coeff(n1), Catch::Matchers::WithinRel(c12 * c23));
        REQUIRE_THAT(diff.coeff(n2), Catch::Matchers::WithinRel(c23));
    }

    SECTION("compute four nodes in a diamond") {
        const auto n1 = create_node<double>();
        constexpr double c12 = 1.234;
        const auto n2 = create_node<double>(n1, c12);
        constexpr double c13 = 2.345;
        const auto n3 = create_node<double>(n1, c13);
        constexpr double c24 = 3.456;
        constexpr double c34 = 4.567;
        const auto n4 = create_node<double>(n2, c24, n3, c34);

        auto diff = node_differentiator<double>();
        REQUIRE_NOTHROW(diff.compute(n4));
        REQUIRE_THAT(
            diff.coeff(n1), Catch::Matchers::WithinRel(c12 * c24 + c13 * c34));
        REQUIRE_THAT(diff.coeff(n2), Catch::Matchers::WithinRel(c24));
        REQUIRE_THAT(diff.coeff(n3), Catch::Matchers::WithinRel(c34));
        REQUIRE_THAT(diff.coeff(n4), Catch::Matchers::WithinRel(1.0));
    }

    SECTION("compute two nodes separated") {
        const auto n1 = create_node<double>();
        const auto n2 = create_node<double>();

        auto diff = node_differentiator<double>();
        REQUIRE_NOTHROW(diff.compute(n2));
        REQUIRE_THAT(diff.coeff(n1), Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(diff.coeff(n2), Catch::Matchers::WithinRel(1.0));
    }
}
