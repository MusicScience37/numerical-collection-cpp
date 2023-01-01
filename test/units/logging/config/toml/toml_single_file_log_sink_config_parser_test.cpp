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
 * \brief Test of toml_single_file_log_sink_config_parser class.
 */
#include "num_collect/logging/config/toml/toml_single_file_log_sink_config_parser.h"

#include <string>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/logging/config/log_sink_factory_table.h"

TEST_CASE(
    "num_collect::logging::config::toml::toml_single_file_log_sink_config_"
    "parser") {
    using num_collect::logging::config::log_sink_factory_table;
    using num_collect::logging::config::toml::
        toml_single_file_log_sink_config_parser;

    log_sink_factory_table sinks;

    SECTION("create from a configuration") {
        const auto test_toml = std::string_view(R"(
name = "single_file"
type = "single_file"
filepath = "logging/toml_single_file_log_sink_config_parser.log"
)");
        const auto table = toml::parse(test_toml);

        CHECK_NOTHROW((void)toml_single_file_log_sink_config_parser()
                          .parse(table)
                          ->create(sinks));
    }
}
