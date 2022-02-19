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
 * \brief Test of assertions.
 */
#include "num_collect/util/assert.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("NUM_COLLECT_ASSERT") {
    SECTION("condition satisfied") {
        REQUIRE_NOTHROW(NUM_COLLECT_ASSERT(true));
    }

    SECTION("condition not satisfied") {
        REQUIRE_THROWS_WITH(NUM_COLLECT_ASSERT(0 == 1),  // NOLINT
            Catch::Matchers::StartsWith("Assertion failure: 0 == 1"));
    }
}

TEST_CASE("NUM_COLLECT_DEBUG_ASSERT") {
    SECTION("condition satisfied") {
        REQUIRE_NOTHROW(NUM_COLLECT_DEBUG_ASSERT(true));
    }

    SECTION("condition not satisfied") {
#ifndef NDEBUG
        REQUIRE_THROWS_WITH(NUM_COLLECT_DEBUG_ASSERT(0 == 1),  // NOLINT
            Catch::Matchers::StartsWith("Assertion failure: 0 == 1"));
#else
        REQUIRE_NOTHROW(NUM_COLLECT_DEBUG_ASSERT(false));
#endif
    }
}
