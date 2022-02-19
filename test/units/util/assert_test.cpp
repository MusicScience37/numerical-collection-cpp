/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of assertion_decomposer class.
 */
#include "num_collect/util/assert.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_collect/base/exception.h"

TEST_CASE("NUM_COLLECT_ASSERT_IMPL") {
    SECTION("smart pointers") {
        CHECK_NOTHROW(NUM_COLLECT_ASSERT_IMPL(std::make_shared<int>(0)));
        CHECK_NOTHROW(NUM_COLLECT_ASSERT_IMPL(!std::shared_ptr<int>()));

        CHECK_THROWS_WITH(NUM_COLLECT_ASSERT_IMPL(std::shared_ptr<int>()),
            Catch::Matchers::ContainsSubstring("std::shared_ptr<int>()"));
        CHECK_THROWS_WITH(NUM_COLLECT_ASSERT_IMPL(!std::make_shared<int>(0)),
            Catch::Matchers::ContainsSubstring("!std::make_shared<int>(0)"));
    }

    SECTION("comparisons using single operator") {
        CHECK_NOTHROW(NUM_COLLECT_ASSERT_IMPL(1 < 2));
        CHECK_NOTHROW(NUM_COLLECT_ASSERT_IMPL(1 + 1 == 2));

        CHECK_THROWS_WITH(NUM_COLLECT_ASSERT_IMPL(1 + 1 > 2),
            Catch::Matchers::ContainsSubstring("1 + 1 > 2 (2 > 2)"));
        CHECK_THROWS_WITH(NUM_COLLECT_ASSERT_IMPL(2 + 1 == 2 * 1),
            Catch::Matchers::ContainsSubstring("2 + 1 == 2 * 1 (3 == 2)"));
    }

    SECTION("comparisons using two operators") {
        CHECK_NOTHROW(NUM_COLLECT_ASSERT_IMPL(1 < 2 < 3));
        CHECK_NOTHROW(NUM_COLLECT_ASSERT_IMPL(1 < 2 <= 2));

        CHECK_THROWS_WITH(NUM_COLLECT_ASSERT_IMPL(1 < 2 < 2),
            Catch::Matchers::ContainsSubstring("1 < 2 < 2 (1 < 2 < 2)"));
        CHECK_THROWS_WITH(NUM_COLLECT_ASSERT_IMPL(1 < 2 <= 1),
            Catch::Matchers::ContainsSubstring("1 < 2 <= 1 (1 < 2 <= 1)"));
    }
}
