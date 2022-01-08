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
 * \brief Test of log_tag class.
 */
#include "num_collect/logging/log_tag.h"

#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/util/hash_string.h"

TEST_CASE("num_collect::logging::log_tag") {
    using num_collect::hash_string;
    using num_collect::logging::log_tag;

    SECTION("check noexcept") {
        STATIC_REQUIRE(std::is_copy_constructible_v<log_tag>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<log_tag>);
        STATIC_REQUIRE(std::is_copy_assignable_v<log_tag>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<log_tag>);
        STATIC_REQUIRE(std::is_nothrow_destructible_v<log_tag>);
    }

    SECTION("construct") {
        constexpr std::string_view name = "Tag";

        const auto tag = log_tag(name);

        REQUIRE(tag.name() == std::string(name));
        REQUIRE(tag.hash() == hash_string(name));
    }

    SECTION("compare") {
        const auto tag1 = log_tag("Tag1");
        const auto tag2 = log_tag("Tag2");

        REQUIRE(tag1 == tag1);
        REQUIRE(tag1 != tag2);
        REQUIRE(tag1 < tag2);
    }
}
