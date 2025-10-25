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
 * \brief Test of toml_log_config_parser class.
 */
#include "num_collect/logging/config/toml/toml_log_config_parser.h"

#include <fstream>
#include <string_view>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "../../mock_log_sink.h"
#include "../mock_log_sink_factory.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_config_node.h"
#include "num_collect/logging/log_tag_view.h"
#include "trompeloeil_catch2.h"

TEST_CASE("num_collect::logging::config::toml::impl::require_log_level") {
    using num_collect::logging::log_level;
    using num_collect::logging::config::toml::impl::require_log_level;

    const auto test_toml = std::string_view(R"(
[test]
trace = "trace"
debug = "debug"
iteration = "iteration"
summary = "summary"
info = "info"
warning = "warning"
error = "error"
critical = "critical"
off = "off"

invalid1 = "invalid"
invalid2 = 123
)");
    const auto test_table = toml::parse(test_toml);

    SECTION("valid log levels") {
        const auto dict = std::unordered_map<log_level, std::string>{
            {log_level::trace, "trace"}, {log_level::debug, "debug"},
            {log_level::iteration, "iteration"},
            {log_level::summary, "summary"}, {log_level::info, "info"},
            {log_level::warning, "warning"}, {log_level::error, "error"},
            {log_level::critical, "critical"}, {log_level::off, "off"}};

        for (const auto& [level, str] : dict) {
            INFO("level = " << static_cast<int>(level));
            INFO("str = " << str);
            CHECK(require_log_level(test_table, fmt::format("test.{}", str),
                      "config_name") == level);
        }
    }

    SECTION("invalid log levels") {
        CHECK_THROWS((void)require_log_level(
            test_table, "test.invalid1", "config_name"));
        CHECK_THROWS((void)require_log_level(
            test_table, "test.invalid2", "config_name"));
        CHECK_THROWS((void)require_log_level(
            test_table, "test.not_exist", "config_name"));
        CHECK_THROWS((void)require_log_level(
            test_table, "not_exist.not_exist", "config_name"));
    }
}

TEST_CASE("num_collect::logging::config::toml::impl::parse_log_tag_config_to") {
    using num_collect::logging::edit_log_tag_config;
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag;
    using num_collect::logging::log_tag_config;
    using num_collect::logging::log_tag_view;
    using num_collect::logging::config::log_sink_factory_table;
    using num_collect::logging::config::toml::impl::parse_log_tag_config_to;
    using num_collect_test::logging::mock_log_sink;
    using num_collect_test::logging::config::mock_log_sink_factory;
    using trompeloeil::_;

    const auto test_toml = std::string_view(R"(
[test.all]
tag = "log_tag"
sink = "sink1"
output_log_level = "iteration"
output_log_level_in_child_iterations = "summary"
iteration_output_period = 2
iteration_label_period = 3

[test.tag_only]
tag = ""

[test.invalid_sink]
tag = ""
sink = "not_exist"
)");
    const auto test_table = toml::parse(test_toml);

    log_sink_factory_table sinks;

    const auto name1 = std::string("sink1");
    const auto sink1 = std::make_shared<mock_log_sink>();
    const auto factory1 = std::make_shared<mock_log_sink_factory>();
    ALLOW_CALL(*factory1, create(_)).RETURN(sink1->to_log_sink());
    sinks.append(name1, factory1);

    SECTION("all configurations") {
        const auto* table = test_table.at_path("test.all").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        auto config = edit_log_tag_config(
            log_tag_view{"num_collect_test::logging::config:"
                         ":toml::impl::require_log_level"});
        CHECK_NOTHROW(parse_log_tag_config_to(config, *table, sinks));
        CHECK(config.output_log_level() == log_level::iteration);
        CHECK(config.output_log_level_in_child_iterations() ==
            log_level::summary);
        CHECK(config.iteration_output_period() == 2);
        CHECK(config.iteration_label_period() == 3);
    }

    SECTION("tag only") {
        const auto* table = test_table.at_path("test.tag_only").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        auto config = edit_log_tag_config(
            log_tag_view{"num_collect_test::logging::config:"
                         ":toml::impl::require_log_level"});
        CHECK_NOTHROW(parse_log_tag_config_to(config, *table, sinks));
    }

    SECTION("sink not found") {
        const auto* table = test_table.at_path("test.invalid_sink").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        auto config = edit_log_tag_config(
            log_tag_view{"num_collect_test::logging::config:"
                         ":toml::impl::require_log_level"});
        CHECK_THROWS_WITH(parse_log_tag_config_to(config, *table, sinks),
            Catch::Matchers::ContainsSubstring("not_exist"));
    }
}

TEST_CASE("num_collect::logging::config::toml::toml_log_config_parser") {
    using num_collect::logging::get_log_tag_config;
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag_view;
    using num_collect::logging::config::toml::toml_log_config_parser;

    const auto test_toml = std::string_view(R"(
# valid config
[[test.valid.num_collect.logging.tag_configs]]
tag = "num_collect::logging::config::toml::toml_log_config_parser1"
sink = "sink2"
output_log_level = "iteration"

[[test.valid.num_collect.logging.tag_configs]]
tag = "num_collect::logging::config::toml::toml_log_config_parser2"
output_log_level = "summary"

[[test.valid.num_collect.logging.sinks]]
name = "sink1"
type = "single_file"
filepath = "logging/toml_log_config_parser1.log"

[[test.valid.num_collect.logging.sinks]]
name = "sink2"
type = "single_file"
filepath = "logging/toml_log_config_parser2.log"

# invalid type for sinks (array)
[test.invalid_sink_type.num_collect.logging]
sinks = 123

# invalid type for sinks (elements)
[test.invalid_sink_element_type.num_collect.logging]
sinks = [123]

# invalid type for log_tag_config (array)
[test.invalid_tag_config_type.num_collect.logging]
tag_configs = 123

# invalid type for log_tag_config (element)
[test.invalid_tag_config_element_type.num_collect.logging]
tag_configs = [123]
)");
    const auto test_table = toml::parse(test_toml);

    SECTION("valid config") {
        const auto* table = test_table.at_path("test.valid").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_NOTHROW(toml_log_config_parser().parse_from_table(*table));
        CHECK(get_log_tag_config(log_tag_view("num_collect::logging::config::"
                                              "toml::toml_log_config_parser1"))
                  .output_log_level() == log_level::iteration);
        CHECK(get_log_tag_config(log_tag_view("num_collect::logging::config::"
                                              "toml::toml_log_config_parser2"))
                  .output_log_level() == log_level::summary);
    }

    SECTION("invalid type for sinks (array)") {
        const auto* table =
            test_table.at_path("test.invalid_sink_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(toml_log_config_parser().parse_from_table(*table));
    }

    SECTION("invalid type for sinks (element)") {
        const auto* table =
            test_table.at_path("test.invalid_sink_element_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(toml_log_config_parser().parse_from_table(*table));
    }

    SECTION("invalid type for log_tag_config (array)") {
        const auto* table =
            test_table.at_path("test.invalid_tag_config_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(toml_log_config_parser().parse_from_table(*table));
    }

    SECTION("invalid type for log_tag_config (element)") {
        const auto* table =
            test_table.at_path("test.invalid_tag_config_element_type")
                .as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(toml_log_config_parser().parse_from_table(*table));
    }

    SECTION("parse from file") {
        const std::string filepath = "logging_toml_log_config_parser_test.toml";
        {
            std::ofstream stream{filepath};
            stream << R"(
# valid config
[[num_collect.logging.tag_configs]]
tag = "num_collect::logging::config::toml::toml_log_config_parser3"
sink = "sink3"
output_log_level = "debug"

[[num_collect.logging.sinks]]
name = "sink3"
type = "single_file"
filepath = "logging/toml_log_config_parser3.log"
)";
        }
        CHECK_NOTHROW(toml_log_config_parser().parse_from_file(filepath));
        CHECK(get_log_tag_config(log_tag_view("num_collect::logging::config::"
                                              "toml::toml_log_config_parser3"))
                  .output_log_level() == log_level::debug);
    }

    SECTION("parse from text") {
        const std::string_view config_text = R"(
# valid config
[[num_collect.logging.tag_configs]]
tag = "num_collect::logging::config::toml::toml_log_config_parser4"
sink = "sink4"
output_log_level = "iteration"

[[num_collect.logging.sinks]]
name = "sink4"
type = "single_file"
filepath = "logging/toml_log_config_parser44.log"
)";
        CHECK_NOTHROW(toml_log_config_parser().parse_from_text(config_text));
        CHECK(get_log_tag_config(log_tag_view("num_collect::logging::config::"
                                              "toml::toml_log_config_parser4"))
                  .output_log_level() == log_level::iteration);
    }
}
