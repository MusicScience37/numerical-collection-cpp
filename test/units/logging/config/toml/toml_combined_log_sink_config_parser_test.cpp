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
 * \brief Test of toml_combined_log_sink_config_parser class.
 */
#include "num_collect/logging/config/toml/toml_combined_log_sink_config_parser.h"

#include <string>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "../../mock_log_sink.h"
#include "../mock_log_sink_factory.h"
#include "num_collect/logging/config/log_sink_factory_table.h"

TEST_CASE(
    "num_collect::logging::config::toml::toml_combined_log_sink_config_"
    "parser") {
    using num_collect::logging::config::log_sink_factory_table;
    using num_collect::logging::config::toml::
        toml_combined_log_sink_config_parser;
    using num_collect_test::logging::mock_log_sink;
    using num_collect_test::logging::config::mock_log_sink_factory;
    using trompeloeil::_;

    log_sink_factory_table sinks;

    const auto name1 = std::string("sink1");
    const auto sink1 = std::make_shared<mock_log_sink>();
    const auto factory1 = std::make_shared<mock_log_sink_factory>();
    sinks.append(name1, factory1);
    // NOLINTNEXTLINE
    ALLOW_CALL(*factory1, create(_)).RETURN(sink1);

    const auto name2 = std::string("sink2");
    const auto sink2 = std::make_shared<mock_log_sink>();
    const auto factory2 = std::make_shared<mock_log_sink_factory>();
    sinks.append(name2, factory2);
    // NOLINTNEXTLINE
    ALLOW_CALL(*factory2, create(_)).RETURN(sink2);

    SECTION("create from a configuration") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_names = ["sink1", "sink2"]
output_log_levels = ["trace", "debug"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_NOTHROW(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("try to create from a configuration without log sink") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
output_log_levels = ["trace", "debug"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION(
        "try to create from a configuration with invalid type of log sinks") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_names = "sink1"
output_log_levels = ["trace", "debug"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION(
        "try to create from a configuration with invalid type of log sink "
        "elements") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_names = [["sink1"]]
output_log_levels = ["trace", "debug"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("try to create from a configuration with invalid log sink") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_name = ["sink1", "nonexisting log sink"]
output_log_levels = ["trace", "debug"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("try to create from a configuration without log level") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_names = ["sink1", "sink2"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION(
        "try to create from a configuration with invalid type of log levels") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_names = ["sink1", "sink2"]
output_log_levels = "trace"
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION(
        "try to create from a configuration with invalid type of log level "
        "elements") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_names = ["sink1", "sink2"]
output_log_levels = [["trace", "debug"]]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("try to create from a configuration with invalid log levels") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_names = ["sink1", "sink2"]
output_log_levels = ["trace", "iteration_label"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION(
        "try to create from a configuration with different number of log sinks "
        "and log levels") {
        const auto test_toml = std::string_view(R"(
name = "combined"
type = "combined"
inner_sink_name = ["sink1", "sink2"]
output_log_levels = ["trace", "debug", "info"]
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_combined_log_sink_config_parser().parse(table)->create(
                sinks));
    }
}
