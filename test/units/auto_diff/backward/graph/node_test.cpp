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
#include "num_collect/auto_diff/backward/graph/node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::auto_diff::backward::graph::node") {
    using num_collect::auto_diff::backward::graph::child_node;
    using num_collect::auto_diff::backward::graph::create_node;
    using num_collect::auto_diff::backward::graph::node;
    using num_collect::auto_diff::backward::graph::node_ptr;

    SECTION("construct") {
        const auto n1 = create_node<double>();
        const auto n2 = create_node<double>();
        constexpr auto c1 = 1.234;
        constexpr auto c2 = 2.345;

        const auto n = create_node<double>(n1, c1, n2, c2);

        REQUIRE(n->children().size() == 2);
        REQUIRE(n->children()[0].node() == n1);
        REQUIRE(n->children()[0].sensitivity() == c1);
        REQUIRE(n->children()[1].node() == n2);
        REQUIRE(n->children()[1].sensitivity() == c2);
    }

    SECTION("try to construct with nullptr") {
        REQUIRE_THROWS((void)create_node<double>(node_ptr<double>(), 0.0));
    }
}
