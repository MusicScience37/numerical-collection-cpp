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
 * \brief Test of toml_console_log_sink_config_parser class.
 */
#include "num_collect/logging/config/toml/toml_console_log_sink_config_parser.h"

#include <string>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/logging/config/log_sink_factory_table.h"

TEST_CASE(
    "num_collect::logging::config::toml::toml_console_log_sink_config_parser") {
    using num_collect::logging::config::log_sink_factory_table;
    using num_collect::logging::config::toml::
        toml_console_log_sink_config_parser;

    log_sink_factory_table sinks;

    SECTION("create from default configuration") {
        const auto test_toml = std::string_view(R"(
name = "console"
type = "console"
)");
        const auto table = toml::parse(test_toml);

        CHECK_NOTHROW(
            (void)toml_console_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("create with use_color") {
        const auto test_toml = std::string_view(R"(
name = "console"
type = "console"
use_color = false
)");
        const auto table = toml::parse(test_toml);

        CHECK_NOTHROW(
            (void)toml_console_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("create with invalid type") {
        const auto test_toml = std::string_view(R"(
name = "console"
type = "console"
use_color = "abc"
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_console_log_sink_config_parser().parse(table)->create(
                sinks));
    }
}
