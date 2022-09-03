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
 * \brief Test of load_logging_config function.
 */
#include "num_collect/logging/load_logging_config.h"

#include <fstream>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"

TEST_CASE("num_collect::logging::load_logging_config") {
    using num_collect::logging::load_logging_config;
    using num_collect::logging::log_config;
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag_view;

    const auto filepath =
        std::string("logging_impl_toml_config_load_logging_config_test.toml");

    SECTION("valid") {
        const auto test_toml = std::string_view(R"(
# valid config
[[num_collect.logging.tag_configs]]
tag = "num_collect_test::logging::impl::toml_config::load_logging_config1"
sink = "sink2"
output_log_level = "iteration"

[[num_collect.logging.tag_configs]]
tag = "num_collect_test::logging::impl::toml_config::load_logging_config2"
output_log_level = "summary"

[[num_collect.logging.sinks]]
name = "sink1"
type = "single_file"
filepath = "logging_impl_toml_config_load_logging_config_test1.log"

[[num_collect.logging.sinks]]
name = "sink2"
type = "single_file"
filepath = "logging_impl_toml_config_load_logging_config_test2.log"
)");
        {
            std::ofstream stream{filepath};
            stream << test_toml;
            stream.flush();
            REQUIRE(stream);
        }

        CHECK_NOTHROW(load_logging_config(filepath));
    }

    SECTION("invalid") {
        const auto test_toml = std::string_view(R"(
# valid config
[[num_collect.logging.tag_configs]]
tag = "num_collect_test::logging::impl::toml_config::load_logging_config1"
sink = "sink2"
output_log_level = "iteration"

[[num_collect.logging.tag_configs]]
tag = "num_collect_test::logging::impl::toml_config::load_logging_config2"
output_log_level = "summary"
)");
        {
            std::ofstream stream{filepath};
            stream << test_toml;
            stream.flush();
            REQUIRE(stream);
        }

        CHECK_THROWS_WITH(load_logging_config(filepath),
            Catch::Matchers::ContainsSubstring(filepath));
        CHECK(log_config::instance()
                  .get_config_of(
                      log_tag_view("num_collect_test::logging::impl::toml_"
                                   "config::load_logging_config1"))
                  .output_log_level() == log_level::iteration);
        CHECK(log_config::instance()
                  .get_config_of(
                      log_tag_view("num_collect_test::logging::impl::toml_"
                                   "config::load_logging_config2"))
                  .output_log_level() == log_level::summary);
    }
}
