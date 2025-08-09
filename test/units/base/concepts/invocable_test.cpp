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
 * \brief Test of invocable concept.
 */
#include "num_collect/base/concepts/invocable.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::concepts::invocable") {
    using num_collect::concepts::invocable;

    SECTION("check some types") {
        STATIC_REQUIRE(invocable<void (*)()>);
        STATIC_REQUIRE(invocable<void (*)(int), int>);
        STATIC_REQUIRE_FALSE(invocable<void (*)(int), std::string>);
    }
}
