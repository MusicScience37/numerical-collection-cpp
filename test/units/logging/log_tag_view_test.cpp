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
 * \brief Test of log_tag_view class.
 */
#include "num_collect/logging/log_tag_view.h"

#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/util/hash_string.h"

TEST_CASE("num_collect::logging::log_tag_view") {
    using num_collect::hash_string;
    using num_collect::logging::log_tag_view;

    SECTION("check noexcept") {
        STATIC_REQUIRE(
            std::is_nothrow_constructible_v<log_tag_view, std::string_view>);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<log_tag_view>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<log_tag_view>);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<log_tag_view>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<log_tag_view>);
        STATIC_REQUIRE(std::is_nothrow_destructible_v<log_tag_view>);
    }

    SECTION("construct in constexpr") {
        constexpr std::string_view name = "Tag";

        constexpr auto tag = log_tag_view(name);

        STATIC_REQUIRE(tag.name().data() == name.data());
        STATIC_REQUIRE(tag.hash() == hash_string(name));
    }

    SECTION("construct not in constexpr") {
        const std::string name = "Tag";

        const auto tag = log_tag_view(name);

        REQUIRE(std::string(tag.name()) == name);
        REQUIRE(tag.hash() == hash_string(name));
    }
}
