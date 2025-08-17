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
 * \brief Test for log_tag_config_node class.
 */
#include "num_collect/logging/log_tag_config_node.h"

#include <catch2/catch_test_macros.hpp>

#include "mock_log_sink.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"

TEST_CASE("num_collect::logging::log_tag_config_node") {
    using num_collect::logging::edit_log_tag_config;
    using num_collect::logging::get_log_tag_config;
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag;
    using num_collect::logging::log_tag_config_node;

    SECTION("set parameters") {
        const auto parent_tag =
            log_tag("num_collect_test::logging::log_tag_config_node");
        const auto child_tag =
            log_tag("num_collect_test::logging::log_tag_config_node::child");
        // Create internal data of child node for test.
        (void)edit_log_tag_config(child_tag);
        auto parent = edit_log_tag_config(parent_tag);

        SECTION("sink") {
            auto sink =
                std::make_shared<num_collect_test::logging::mock_log_sink>();
            CHECK_NOTHROW(parent.sink(sink->to_log_sink()));
        }

        SECTION("output_log_level") {
            const auto valid_values = std::vector<log_level>{log_level::trace,
                log_level::debug, log_level::iteration, log_level::summary,
                log_level::info, log_level::warning, log_level::error,
                log_level::critical, log_level::off};
            for (const log_level value : valid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK(
                    parent.output_log_level(value).output_log_level() == value);
                CHECK(
                    get_log_tag_config(child_tag).output_log_level() == value);
            }

            const auto invalid_values = std::vector<log_level>{
                log_level::iteration_label,
                static_cast<log_level>(static_cast<int>(log_level::off) + 1)};
            for (const log_level value : invalid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK_THROWS(parent.output_log_level(value));
            }
        }

        SECTION("output_log_level_in_child_iterations") {
            const auto valid_values = std::vector<log_level>{log_level::trace,
                log_level::debug, log_level::iteration, log_level::summary,
                log_level::info, log_level::warning, log_level::error,
                log_level::critical, log_level::off};
            for (const log_level value : valid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK(parent.output_log_level_in_child_iterations(value)
                          .output_log_level_in_child_iterations() == value);
                CHECK(get_log_tag_config(child_tag)
                          .output_log_level_in_child_iterations() == value);
            }

            const auto invalid_values = std::vector<log_level>{
                log_level::iteration_label,
                static_cast<log_level>(static_cast<int>(log_level::off) + 1)};
            for (const log_level value : invalid_values) {
                INFO(fmt::format("value = {}", value));
                CHECK_THROWS(
                    parent.output_log_level_in_child_iterations(value));
            }
        }

        SECTION("iteration_output_period") {
            CHECK_THROWS(parent.iteration_output_period(0));
            CHECK_NOTHROW(parent.iteration_output_period(1));

            const num_collect::index_type val = 123;
            CHECK(
                parent.iteration_output_period(val).iteration_output_period() ==
                val);
            CHECK(
                get_log_tag_config(child_tag).iteration_output_period() == val);
        }

        SECTION("iteration_label_period") {
            CHECK_THROWS(parent.iteration_label_period(0));
            CHECK_NOTHROW(parent.iteration_label_period(1));

            const num_collect::index_type val = 123;
            CHECK(parent.iteration_label_period(val).iteration_label_period() ==
                val);
            CHECK(
                get_log_tag_config(child_tag).iteration_label_period() == val);
        }
    }
}
