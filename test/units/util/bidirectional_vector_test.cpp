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
 * \brief Test of exceptions.
 */
#include "num_collect/util/bidirectional_vector.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::bidirectional_vector") {
    using num_collect::bidirectional_vector;

    SECTION("default constructor") {
        const auto vec = bidirectional_vector<int>();

        REQUIRE(vec.container().empty());
        REQUIRE(vec.empty());
    }

    SECTION("constructor with members") {
        const auto cont = std::deque<int>{3, 5, 7};
        const num_collect::index_type origin = -3;

        const auto vec = bidirectional_vector<int>(cont, origin);

        REQUIRE(vec.container() == cont);
        REQUIRE_FALSE(vec.empty());
        REQUIRE(vec.min_index() == origin);
        REQUIRE(vec.max_index() == -1);
    }

    SECTION("access values with checks (const object)") {
        const auto cont = std::deque<int>{3, 5, 7};
        const num_collect::index_type origin = -3;

        const auto vec = bidirectional_vector<int>(cont, origin);

        REQUIRE_THROWS(vec.at(-4));
        REQUIRE(vec.at(-3) == cont.at(0));
        REQUIRE(vec.at(-2) == cont.at(1));
        REQUIRE(vec.at(-1) == cont.at(2));
        REQUIRE_THROWS(vec.at(0));
    }

    SECTION("access values with checks (non-const object)") {
        const auto cont = std::deque<int>{3, 5, 7};
        const num_collect::index_type origin = -3;

        auto vec = bidirectional_vector<int>(cont, origin);

        REQUIRE_THROWS(vec.at(-4));
        REQUIRE(vec.at(-3) == cont.at(0));
        REQUIRE(vec.at(-2) == cont.at(1));
        REQUIRE(vec.at(-1) == cont.at(2));
        REQUIRE_THROWS(vec.at(0));
    }

    SECTION("access values without checks (const object)") {
        const auto cont = std::deque<int>{3, 5};
        const num_collect::index_type origin = 1;

        const auto vec = bidirectional_vector<int>(cont, origin);

        REQUIRE(vec[1] == cont.at(0));
        REQUIRE(vec[2] == cont.at(1));
    }

    SECTION("access values without checks (non-const object)") {
        const auto cont = std::deque<int>{3, 5};
        const num_collect::index_type origin = 1;

        auto vec = bidirectional_vector<int>(cont, origin);

        REQUIRE(vec[1] == cont.at(0));
        REQUIRE(vec[2] == cont.at(1));
    }

    SECTION("access values preparing them if needed") {
        auto vec = bidirectional_vector<int>();

        constexpr num_collect::index_type ind1 = 3;
        constexpr int value1 = 7;
        vec.get_or_prepare(ind1) = value1;

        REQUIRE(vec.min_index() == ind1);
        REQUIRE(vec.max_index() == ind1);
        REQUIRE(vec.at(ind1) == value1);

        constexpr num_collect::index_type ind2 = -15;
        constexpr int value2 = -34;
        vec.get_or_prepare(ind2) = value2;

        REQUIRE(vec.min_index() == ind2);
        REQUIRE(vec.max_index() == ind1);
        REQUIRE(vec.at(ind1) == value1);
        REQUIRE(vec.at(ind2) == value2);
        REQUIRE(vec.at(0) == 0);
    }

    SECTION("change size") {
        auto vec = bidirectional_vector<int>();

        constexpr num_collect::index_type min_ind = 5;
        constexpr num_collect::index_type max_ind = 7;
        vec.resize(min_ind, max_ind);

        REQUIRE(vec.min_index() == min_ind);
        REQUIRE(vec.max_index() == max_ind);
        REQUIRE(vec.at(min_ind) == 0);
        REQUIRE(vec.at(max_ind) == 0);

        SECTION("change size to left") {
            constexpr num_collect::index_type min_ind2 = -12;
            constexpr num_collect::index_type max_ind2 = -3;
            constexpr int value = 37;
            vec.resize(min_ind2, max_ind2, value);

            REQUIRE(vec.min_index() == min_ind2);
            REQUIRE(vec.max_index() == max_ind2);
            REQUIRE(vec.at(min_ind2) == value);
            REQUIRE(vec.at(max_ind2) == value);
        }

        SECTION("change size to right") {
            constexpr num_collect::index_type min_ind2 = 6;
            constexpr num_collect::index_type max_ind2 = 13;
            static_assert(
                min_ind < min_ind2 && min_ind2 < max_ind && max_ind < max_ind2);
            constexpr int value = 41;
            vec.resize(min_ind2, max_ind2, value);

            REQUIRE(vec.min_index() == min_ind2);
            REQUIRE(vec.max_index() == max_ind2);
            REQUIRE(vec.at(min_ind2) == 0);
            REQUIRE(vec.at(max_ind) == 0);
            REQUIRE(vec.at(max_ind + 1) == value);
            REQUIRE(vec.at(max_ind2) == value);
        }
    }
}
