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
 * \brief Test of parse_log_tag_element function.
 */
#include "num_collect/logging/impl/parse_log_tag_element.h"

#include <string>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::impl::parse_log_tag_element") {
    using num_collect::logging::impl::parse_log_tag_element;

    using res_type = std::tuple<std::string, std::string>;

    SECTION("parse valid elements") {
        const auto test = [](const std::string& input,
                              const std::string& expected_element,
                              const std::string& expected_remaining) {
            INFO("input = " << input);
            const auto [element, remaining] = parse_log_tag_element(input);
            CHECK(std::string(element) == expected_element);
            CHECK(std::string(remaining) == expected_remaining);
        };

        test("09AZaz", "09AZaz", "");

        test("test/", "test", "/");
        test("test:", "test", ":");
        test("test@", "test", "@");
        test("test[", "test", "[");
        test("test`", "test", "`");
        test("test{", "test", "{");
    }

    SECTION("parse invalid elements") {
        CHECK_THROWS((void)parse_log_tag_element(""));

        CHECK_THROWS((void)parse_log_tag_element("/"));
        CHECK_THROWS((void)parse_log_tag_element(":"));
        CHECK_THROWS((void)parse_log_tag_element("@"));
        CHECK_THROWS((void)parse_log_tag_element("["));
        CHECK_THROWS((void)parse_log_tag_element("`"));
        CHECK_THROWS((void)parse_log_tag_element("{"));
    }
}
