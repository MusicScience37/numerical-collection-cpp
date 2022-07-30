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
 * \brief Test of load_logging_config_toml function.
 */
#include "num_collect/logging/impl/load_logging_config_toml.h"

#include <cstdint>
#include <ostream>
#include <string_view>
#include <tuple>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_collect/logging/colored_console_log_sink.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_sink_base.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"

TEST_CASE("num_collect::logging::impl::toml_config::require_value") {
    using num_collect::logging::impl::toml_config::require_value;

    const auto test_toml = std::string_view(R"(
[test.success]
str = "abc"
int_val = 123

[test.too_large_integer]
int_val = 1000000000000000
)");
    const auto test_table = toml::parse(test_toml);

    SECTION("get successfully") {
        CHECK(require_value<std::string>(test_table, "test.success.str",
                  "config_name", "type_name") == "abc");
        CHECK(require_value<std::int32_t>(test_table, "test.success.int_val",
                  "config_name", "type_name") == 123);  // NOLINT
    }

    SECTION("non-existing node") {
        CHECK_THROWS_WITH((void)require_value<std::string>(test_table,
                              "test.not_exist.str", "config_name", "type_name"),
            Catch::Matchers::ContainsSubstring(
                "Configuration config_name is required."));
    }

    SECTION("type error") {
        CHECK_THROWS_WITH((void)require_value<std::int32_t>(test_table,
                              "test.success.str", "config_name", "type_name"),
            Catch::Matchers::ContainsSubstring(
                "Configuration config_name must be type_name."));
    }

    SECTION("too large integer") {
        CHECK_THROWS((void)require_value<std::int32_t>(test_table,
            "test.too_large_integer.int_val", "config_name", "type_name"));
    }
}

TEST_CASE("num_collect::logging::impl::toml_config::require_log_level") {
    using num_collect::logging::log_level;
    using num_collect::logging::impl::toml_config::require_log_level;

    const auto test_toml = std::string_view(R"(
[test]
trace = "trace"
iteration = "iteration"
iteration_label = "iteration_label"
summary = "summary"
info = "info"
warning = "warning"
error = "error"
off = "off"
invalid1 = "invalid"
invalid2 = 123
)");
    const auto test_table = toml::parse(test_toml);

    SECTION("valid log levels") {
        const auto dict = std::unordered_map<log_level, std::string>{
            {log_level::trace, "trace"}, {log_level::iteration, "iteration"},
            {log_level::iteration_label, "iteration_label"},
            {log_level::summary, "summary"}, {log_level::info, "info"},
            {log_level::warning, "warning"}, {log_level::error, "error"},
            {log_level::off, "off"}};

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

TEST_CASE("num_collect::logging::impl::toml_config::parse_log_sink_config") {
    using num_collect::logging::impl::toml_config::parse_log_sink_config;

    const auto test_toml = std::string_view(R"(
[test.valid_colored_console]
name = "colored_console_sink"
type = "colored_console"

[test.valid_single_file]
name = "single_file_sink"
type = "single_file"
filepath = "logging_impl_toml_config_parse_log_sink_config_test.log"

[test.no_name]
type = "colored_console"

[test.no_type]
name = "colored_console_sink"

[test.invalid_type]
name = "colored_console_sink"
type = "invalid"

[test.single_file_no_filepath]
name = "single_file_sink"
type = "single_file"
)");
    const auto test_table = toml::parse(test_toml);

    SECTION("valid colored_console config") {
        const auto* table =
            test_table.at_path("test.valid_colored_console").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_NOTHROW((void)parse_log_sink_config(*table));
    }

    SECTION("valid single_file config") {
        const auto* table =
            test_table.at_path("test.valid_single_file").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_NOTHROW((void)parse_log_sink_config(*table));
    }

    SECTION("no name") {
        const auto* table = test_table.at_path("test.no_name").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS_WITH((void)parse_log_sink_config(*table),
            Catch::Matchers::ContainsSubstring("name"));
    }

    SECTION("no type") {
        const auto* table = test_table.at_path("test.no_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS_WITH((void)parse_log_sink_config(*table),
            Catch::Matchers::ContainsSubstring("type"));
    }

    SECTION("invalid type") {
        const auto* table = test_table.at_path("test.invalid_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS_WITH((void)parse_log_sink_config(*table),
            Catch::Matchers::ContainsSubstring("type"));
    }

    SECTION("no filepath in single_file") {
        const auto* table =
            test_table.at_path("test.single_file_no_filepath").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS_WITH((void)parse_log_sink_config(*table),
            Catch::Matchers::ContainsSubstring("filepath"));
    }
}

TEST_CASE("num_collect::logging::impl::toml_config::parse_log_tag_config") {
    using num_collect::logging::colored_console_log_sink;
    using num_collect::logging::log_level;
    using num_collect::logging::log_sink_base;
    using num_collect::logging::log_tag;
    using num_collect::logging::log_tag_config;
    using num_collect::logging::impl::toml_config::parse_log_tag_config;

    std::unordered_map<std::string, std::shared_ptr<log_sink_base>> sinks;
    sinks.try_emplace(
        "sink1", std::make_shared<colored_console_log_sink>(stdout));
    sinks.try_emplace(
        "sink2", std::make_shared<colored_console_log_sink>(stdout));

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

    SECTION("all configurations") {
        const auto* table = test_table.at_path("test.all").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        log_tag tag{""};
        log_tag_config config;
        CHECK_NOTHROW(
            std::tie(tag, config) = parse_log_tag_config(*table, sinks));
        CHECK(tag.name() == "log_tag");
        CHECK(config.sink() == sinks.at("sink1"));
        CHECK(config.output_log_level() == log_level::iteration);
        CHECK(config.output_log_level_in_child_iterations() ==
            log_level::summary);
        CHECK(config.iteration_output_period() == 2);
        CHECK(config.iteration_label_period() == 3);
    }

    SECTION("tag only") {
        const auto* table = test_table.at_path("test.tag_only").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        log_tag tag{"abc"};
        log_tag_config config;
        CHECK_NOTHROW(
            std::tie(tag, config) = parse_log_tag_config(*table, sinks));
        CHECK(tag.name() == "");  // NOLINT
    }

    SECTION("sink not found") {
        const auto* table = test_table.at_path("test.invalid_sink").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS_WITH((void)parse_log_tag_config(*table, sinks),
            Catch::Matchers::ContainsSubstring("not_exist"));
    }
}

TEST_CASE("num_collect::logging::impl::toml_config::load_logging_config_toml") {
    using num_collect::logging::log_config;
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag_view;
    using num_collect::logging::impl::toml_config::load_logging_config_toml;

    const auto test_toml = std::string_view(R"(
# valid config
[[test.valid.num_collect.logging.tag_configs]]
tag = "num_collect_test::logging::impl::toml_config::load_logging_config_toml1"
sink = "sink2"
output_log_level = "iteration"

[[test.valid.num_collect.logging.tag_configs]]
tag = "num_collect_test::logging::impl::toml_config::load_logging_config_toml2"
output_log_level = "summary"

[[test.valid.num_collect.logging.sinks]]
name = "sink1"
type = "single_file"
filepath = "logging_impl_toml_config_load_logging_config_toml_test1.log"

[[test.valid.num_collect.logging.sinks]]
name = "sink2"
type = "single_file"
filepath = "logging_impl_toml_config_load_logging_config_toml_test2.log"

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
        CHECK_NOTHROW(load_logging_config_toml(*table));
        CHECK(log_config::instance()
                  .get_config_of(
                      log_tag_view("num_collect_test::logging::impl::toml_"
                                   "config::load_logging_config_toml1"))
                  .output_log_level() == log_level::iteration);
        CHECK(log_config::instance()
                  .get_config_of(
                      log_tag_view("num_collect_test::logging::impl::toml_"
                                   "config::load_logging_config_toml2"))
                  .output_log_level() == log_level::summary);
    }

    SECTION("invalid type for sinks (array)") {
        const auto* table =
            test_table.at_path("test.invalid_sink_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(load_logging_config_toml(*table));
    }

    SECTION("invalid type for sinks (element)") {
        const auto* table =
            test_table.at_path("test.invalid_sink_element_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(load_logging_config_toml(*table));
    }

    SECTION("invalid type for log_tag_config (array)") {
        const auto* table =
            test_table.at_path("test.invalid_tag_config_type").as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(load_logging_config_toml(*table));
    }

    SECTION("invalid type for log_tag_config (element)") {
        const auto* table =
            test_table.at_path("test.invalid_tag_config_element_type")
                .as_table();
        REQUIRE(static_cast<const void*>(table) != nullptr);
        CHECK_THROWS(load_logging_config_toml(*table));
    }
}
