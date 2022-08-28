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
 * \brief Test of get_output_log_level_str function.
 */
#include "num_collect/logging/formatters/get_log_level_str_for_log.h"

#include <string>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::formatters::get_output_log_level_str") {
    using num_collect::logging::log_level;
    using num_collect::logging::formatters::get_output_log_level_str;

    SECTION("format") {
        const auto dict = std::unordered_map<log_level, std::string>{
            {log_level::trace, "trace"}, {log_level::debug, "debug"},
            {log_level::iteration, "iteration"},
            {log_level::iteration_label, "iteration"},
            {log_level::summary, "summary"}, {log_level::warning, "warning"},
            {log_level::error, "error"}, {log_level::critical, "critical"},
            {static_cast<log_level>(static_cast<int>(log_level::critical) + 1),
                "unknown"}};

        for (const auto& [level, str] : dict) {
            INFO("level = " << static_cast<int>(level));
            INFO("str = " << str);
            CHECK(std::string(get_output_log_level_str(level)) == str);
        }
    }
}
