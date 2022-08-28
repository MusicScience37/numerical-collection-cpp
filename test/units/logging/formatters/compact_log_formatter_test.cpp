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
 * \brief Test of compact_log_formatter class.
 */
#include "num_collect/logging/formatters/compact_log_formatter.h"

#include <string>
#include <vector>

#include <ApprovalTests.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::logging::formatters::compact_log_formatter") {
    using num_collect::index_type;
    using num_collect::logging::log_level;
    using num_collect::logging::formatters::compact_log_formatter;
    using num_collect::util::source_info_view;

    compact_log_formatter formatter{};

    SECTION("format") {
        // 2022/8/28 17:56:14.123456
        const auto time_since_epoch = std::chrono::seconds{1661709374} +
            std::chrono::microseconds{123456};
        const auto time =
            std::chrono::system_clock::time_point(time_since_epoch);
        const auto tag = std::string_view("Tag");
        const auto file_path = std::string_view("/file/path");
        const index_type line = 123;
        const index_type column = 7;
        const auto function_name = std::string_view("function");
        const auto source =
            source_info_view(file_path, line, column, function_name);
        const auto body = std::string_view("body");

        const auto log_levels = std::vector{log_level::trace, log_level::debug,
            log_level::iteration, log_level::iteration_label,
            log_level::summary, log_level::info, log_level::warning,
            log_level::error, log_level::critical, log_level::off};
        fmt::memory_buffer buffer;
        for (const auto level : log_levels) {
            formatter.format(buffer, time, tag, level, source, body);
            buffer.push_back('\n');
        }

        ApprovalTests::Approvals::verify(
            std::string(buffer.data(), buffer.size()));
    }
}
