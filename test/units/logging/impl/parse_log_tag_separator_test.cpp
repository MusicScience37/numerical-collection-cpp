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
 * \brief Test of parse_log_tag_separator function.
 */
#include "num_collect/logging/impl/parse_log_tag_separator.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::impl::parse_log_tag_separator") {
    using num_collect::logging::impl::parse_log_tag_separator;

    SECTION("parse colons") {
        CHECK(std::string(parse_log_tag_separator("::")) == "");  // NOLINT
        CHECK(std::string(parse_log_tag_separator("::a")) == "a");
        CHECK(std::string(parse_log_tag_separator("::tag")) == "tag");
    }

    SECTION("parse periods") {
        CHECK(std::string(parse_log_tag_separator(".")) == "");  // NOLINT
        CHECK(std::string(parse_log_tag_separator(".a")) == "a");
        CHECK(std::string(parse_log_tag_separator(".tag")) == "tag");
    }

    SECTION("invalid inputs") {
        CHECK_THROWS((void)std::string(parse_log_tag_separator("")));
        CHECK_THROWS((void)std::string(parse_log_tag_separator(":")));
        CHECK_THROWS((void)std::string(parse_log_tag_separator(":a")));
        CHECK_THROWS((void)std::string(parse_log_tag_separator("a")));
    }
}
