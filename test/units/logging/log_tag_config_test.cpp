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
 * \brief Test of log_tag_config class.
 */
#include "num_collect/logging/log_tag_config.h"

#include <vector>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "mock_log_sink.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_level.h"

TEST_CASE("num_collect::logging::log_tag_config") {
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag_config;

    SECTION("default config") {
        const auto config = log_tag_config();

        CHECK(config.output_log_level() == log_level::info);
        CHECK(config.iteration_output_period() > 0);
        CHECK(config.iteration_label_period() > 0);
    }

    SECTION("set config") {
        auto config = log_tag_config();

        SECTION("sink") {
            const auto sink =
                std::make_shared<num_collect_test::logging::mock_log_sink>();
            CHECK_NOTHROW(config.sink(sink->to_log_sink()));
        }

        SECTION("output_log_level") {
            const auto valid_values = std::vector<log_level>{log_level::trace,
                log_level::debug, log_level::iteration, log_level::summary,
                log_level::info, log_level::warning, log_level::error,
                log_level::critical, log_level::off};
            for (const log_level value : valid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK(
                    config.output_log_level(value).output_log_level() == value);
            }

            const auto invalid_values = std::vector<log_level>{
                log_level::iteration_label,
                static_cast<log_level>(static_cast<int>(log_level::off) + 1)};
            for (const log_level value : invalid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK_THROWS(config.output_log_level(value));
            }
        }

        SECTION("output_log_level_in_child_iterations") {
            const auto valid_values = std::vector<log_level>{log_level::trace,
                log_level::debug, log_level::iteration, log_level::summary,
                log_level::info, log_level::warning, log_level::error,
                log_level::critical, log_level::off};
            for (const log_level value : valid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK(config.output_log_level_in_child_iterations(value)
                          .output_log_level_in_child_iterations() == value);
            }

            const auto invalid_values = std::vector<log_level>{
                log_level::iteration_label,
                static_cast<log_level>(static_cast<int>(log_level::off) + 1)};
            for (const log_level value : invalid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK_THROWS(
                    config.output_log_level_in_child_iterations(value));
            }
        }

        SECTION("iteration_output_period") {
            CHECK_THROWS(config.iteration_output_period(0));
            CHECK_NOTHROW(config.iteration_output_period(1));

            const num_collect::index_type val = 123;
            CHECK(
                config.iteration_output_period(val).iteration_output_period() ==
                val);
        }

        SECTION("iteration_label_period") {
            CHECK_THROWS(config.iteration_label_period(0));
            CHECK_NOTHROW(config.iteration_label_period(1));

            const num_collect::index_type val = 123;
            CHECK(config.iteration_label_period(val).iteration_label_period() ==
                val);
        }
    }
}
