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
 * \brief Test of log_formatter class.
 */
#include "num_collect/logging/impl/log_formatter.h"

#include <string_view>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_collect/logging/log_level.h"
#include "num_collect/util/index_type.h"

TEST_CASE("num_collect::logging::impl::iso8601_time") {
    using num_collect::logging::impl::iso8601_time;

    SECTION("format") {
        const auto time = std::chrono::system_clock::now();
        REQUIRE_THAT(fmt::format("{}", iso8601_time(time)),
            Catch::Matchers::Matches(
                R"(\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d\.\d\d\d\d\d\d.\d\d\d\d)"));
    }
}

TEST_CASE("num_collect::logging::impl::get_output_log_level_str") {
    using num_collect::logging::log_level;
    using num_collect::logging::impl::get_output_log_level_str;

    SECTION("format") {
        const auto dict = std::unordered_map<log_level, std::string>{
            {log_level::trace, "trace"}, {log_level::iteration, "iter"},
            {log_level::iteration_label, "iter"},
            {log_level::summary, "summary"}, {log_level::warning, "warning"},
            {log_level::error, "error"},
            {static_cast<log_level>(static_cast<int>(log_level::error) + 1),
                "unknown"}};

        for (const auto& [level, str] : dict) {
            INFO("level = " << static_cast<int>(level));
            INFO("str = " << str);
            CHECK(std::string(get_output_log_level_str(level)) == str);
        }
    }
}

TEST_CASE("num_collect::logging::impl::log_formatter") {
    using num_collect::index_type;
    using num_collect::source_info_view;
    using num_collect::logging::log_level;
    using num_collect::logging::impl::log_formatter;

    SECTION("format") {
        const auto time = std::chrono::system_clock::now();
        const auto tag = std::string_view("Tag");
        const auto level = log_level::summary;
        const auto file_path = std::string_view("filepath");
        const index_type line = 123;
        const index_type column = 7;
        const auto function_name = std::string_view("function");
        const auto source =
            source_info_view(file_path, line, column, function_name);
        const auto body = std::string_view("body");

        auto formatter = log_formatter();
        REQUIRE_THAT(
            std::string(formatter.format(time, tag, level, source, body)),
            Catch::Matchers::Matches(
                R"(\[\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d\.\d\d\d\d\d\d.\d\d\d\d\] )"
                R"(\[summary\] \[Tag\] body \(filepath:123:7, function\))"));
    }
}
