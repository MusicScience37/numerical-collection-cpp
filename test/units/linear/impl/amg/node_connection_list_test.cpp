/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of node_connection_list class.
 */
#include "num_collect/linear/impl/amg/node_connection_list.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

TEST_CASE("num_collect::linear::impl::amg::node_connection_list") {
    using num_collect::linear::impl::amg::node_connection_list;

    SECTION("create a list") {
        node_connection_list<> list;
        list.push_back(1);
        list.push_back(2);
        list.finish_current_node();
        list.finish_current_node();
        list.push_back(0);
        list.finish_current_node();

        CHECK(list.num_nodes() == 3);
        CHECK_THAT(list.connected_nodes_to(0),
            Catch::Matchers::RangeEquals(std::vector<int>{1, 2}));
        CHECK_THAT(list.connected_nodes_to(1),
            Catch::Matchers::RangeEquals(std::vector<int>{}));
        CHECK_THAT(list.connected_nodes_to(2),
            Catch::Matchers::RangeEquals(std::vector<int>{0}));

        SECTION("create another list") {
            list.initialize();
            list.push_back(1);
            list.finish_current_node();
            list.finish_current_node();

            CHECK(list.num_nodes() == 2);
            CHECK_THAT(list.connected_nodes_to(0),
                Catch::Matchers::RangeEquals(std::vector<int>{1}));
            CHECK_THAT(list.connected_nodes_to(1),
                Catch::Matchers::RangeEquals(std::vector<int>{}));
        }
    }

    SECTION("transpose a list") {
        node_connection_list<> list;
        list.push_back(1);
        list.push_back(2);
        list.finish_current_node();
        list.push_back(2);
        list.finish_current_node();
        list.push_back(1);
        list.finish_current_node();

        const auto transposed = list.transpose();

        CHECK(transposed.num_nodes() == 3);
        CHECK_THAT(transposed.connected_nodes_to(0),
            Catch::Matchers::RangeEquals(std::vector<int>{}));
        CHECK_THAT(transposed.connected_nodes_to(1),
            Catch::Matchers::RangeEquals(std::vector<int>{0, 2}));
        CHECK_THAT(transposed.connected_nodes_to(2),
            Catch::Matchers::RangeEquals(std::vector<int>{0, 1}));
    }
}
