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
 * \brief Test of helper functions for parsing TOML files.
 */
#include "num_collect/logging/config/toml/toml_helper.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <toml++/toml.h>

TEST_CASE("num_collect::logging::config::toml::require_value") {
    using num_collect::logging::config::toml::require_value;

    SECTION("get an integer") {
        const auto test_toml = std::string_view(R"(
[test_config]
valid_value = 12345
invalid_type_value = "abc"
)");
        const auto table = toml::parse(test_toml);

        CHECK(require_value<int>(table, "test_config.valid_value",
                  "valid_value in test_config", "integer") == 12345);  // NOLINT

        CHECK_THROWS_WITH(
            require_value<int>(table, "test_config.nonexisting_value",
                "nonexisting_value in test_config", "integer"),
            Catch::Matchers::ContainsSubstring(
                "nonexisting_value in test_config"));

        CHECK_THROWS_WITH(
            require_value<int>(table, "test_config.invalid_type_value",
                "invalid_type_value in test_config", "integer"),
            Catch::Matchers::ContainsSubstring("integer"));
    }

    SECTION("get a string") {
        const auto test_toml = std::string_view(R"(
[test_config]
test_value = "abc"
)");
        const auto table = toml::parse(test_toml);

        CHECK(require_value<std::string>(table, "test_config.test_value",
                  "test_value in test_config", "string") == "abc");  // NOLINT

        CHECK_THROWS_WITH(
            require_value<int>(table, "test_config.nonexisting_value",
                "nonexisting_value in test_config", "string"),
            Catch::Matchers::ContainsSubstring(
                "nonexisting_value in test_config"));
    }
}
