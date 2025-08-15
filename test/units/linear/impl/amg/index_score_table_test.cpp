/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of index_score_table class.
 */
#include "num_collect/linear/impl/amg/index_score_table.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::linear::impl::amg::index_score_table") {
    using num_collect::linear::impl::amg::index_score_table;

    using StorageIndex = int;

    SECTION("assign a pair") {
        index_score_table<StorageIndex> table(4);
        table.assign(0, 2);
        table.assign(1, 3);
        table.assign(2, 1);
        table.assign(3, 1);

        CHECK(table.find_max_score_index() == 1);
    }

    SECTION("remove a pair") {
        index_score_table<StorageIndex> table(4);
        table.assign(0, 2);
        table.assign(1, 3);
        table.assign(2, 1);
        table.assign(3, 1);

        table.remove(1);

        CHECK(table.find_max_score_index() == 0);
    }

    SECTION("add a score to a pair") {
        index_score_table<StorageIndex> table(4);
        table.assign(0, 2);
        table.assign(1, 3);
        table.assign(2, 1);
        table.assign(3, 1);

        table.add_score(0, 2);

        CHECK(table.find_max_score_index() == 0);
    }
}
