/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of toml_async_log_sink_config_parser class.
 */
#include "num_collect/logging/config/toml/toml_async_log_sink_config_parser.h"

#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "../../mock_log_sink.h"
#include "../mock_log_sink_factory.h"
#include "num_collect/logging/config/log_sink_factory_table.h"
#include "trompeloeil_catch2.h"

TEST_CASE(
    "num_collect::logging::config::toml::toml_async_log_sink_config_parser") {
    using num_collect::logging::config::log_sink_factory_table;
    using num_collect::logging::config::toml::toml_async_log_sink_config_parser;
    using num_collect_test::logging::mock_log_sink;
    using num_collect_test::logging::config::mock_log_sink_factory;
    using trompeloeil::_;

    log_sink_factory_table sinks;

    const auto name1 = std::string("sink1");
    const auto sink1 = std::make_shared<mock_log_sink>();
    const auto factory1 = std::make_shared<mock_log_sink_factory>();
    sinks.append(name1, factory1);
    // NOLINTNEXTLINE
    ALLOW_CALL(*factory1, create(_)).RETURN(sink1->to_log_sink());

    SECTION("create from a configuration") {
        const auto test_toml = std::string_view(R"(
name = "async"
type = "async"
inner_sink_name = "sink1"
)");
        const auto table = toml::parse(test_toml);

        CHECK_NOTHROW(
            (void)toml_async_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("try to create from a configuration without log sink") {
        const auto test_toml = std::string_view(R"(
name = "async"
type = "async"
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_async_log_sink_config_parser().parse(table)->create(
                sinks));
    }

    SECTION("try to create from a configuration with invalid log sink") {
        const auto test_toml = std::string_view(R"(
name = "async"
type = "async"
inner_sink_name = "invalid"
)");
        const auto table = toml::parse(test_toml);

        CHECK_THROWS(
            (void)toml_async_log_sink_config_parser().parse(table)->create(
                sinks));
    }
}
