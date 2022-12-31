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
 * \brief Test of separate_top_log_tag_element function.
 */
#include "num_collect/logging/impl/separate_top_log_tag_element.h"

#include <string_view>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::impl::separate_top_log_tag_element") {
    using num_collect::logging::impl::separate_top_log_tag_element;

    SECTION("ordinary case with colons") {
        const auto tag = std::string_view("test::log::tag");

        const auto [element, rem] = separate_top_log_tag_element(tag);
        CHECK(element.name() == "test");
        CHECK(std::string(rem) == "log::tag");
    }

    SECTION("ordinary case with a period") {
        const auto tag = std::string_view("test.log::tag");

        const auto [element, rem] = separate_top_log_tag_element(tag);
        CHECK(element.name() == "test");
        CHECK(std::string(rem) == "log::tag");
    }

    SECTION("no separator") {
        const auto tag = std::string_view("test");

        const auto [element, rem] = separate_top_log_tag_element(tag);
        CHECK(element.name() == "test");
        CHECK(std::string(rem) == "");  // NOLINT
    }

    SECTION("invalid tags") {
        // empty level
        CHECK_THROWS(separate_top_log_tag_element(""));
        CHECK_THROWS(separate_top_log_tag_element("::log::tags"));
        CHECK_THROWS(separate_top_log_tag_element("test::"));

        // single colon
        CHECK_THROWS(separate_top_log_tag_element("test:log::tags"));
    }
}
