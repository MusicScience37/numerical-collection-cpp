/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of iteration_logger class.
 */
#include "num_collect/logging/iteration_logger.h"

#include <iterator>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "mock_log_sink.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"

TEST_CASE("num_collect::logging::iteration_logger_item") {
    using num_collect::logging::iteration_logger_item;

    SECTION("format integers") {
        using value_type = num_collect::index_type;
        value_type val = 0;
        const auto get_val = [&val] { return val; };
        iteration_logger_item<value_type, decltype(get_val)> item{
            "abc", get_val};
        constexpr num_collect::index_type width = 7;
        item.width(width);
        CHECK(item.width() == width);

        val = 12345;  // NOLINT

        fmt::memory_buffer buffer;
        item.format_value_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "  12345");

        buffer.clear();
        item.format_label_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "    abc");

        buffer.clear();
        item.format_summary_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "abc=12345");
    }

    SECTION("format string") {
        using value_type = std::string;
        value_type val;
        const auto get_val = [&val] { return val; };
        iteration_logger_item<value_type, decltype(get_val)> item{
            "abc", get_val};
        constexpr num_collect::index_type width = 7;
        item.width(width);
        CHECK(item.width() == width);

        val = "def";

        fmt::memory_buffer buffer;
        item.format_value_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "    def");

        buffer.clear();
        item.format_label_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "    abc");

        buffer.clear();
        item.format_summary_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "abc=def");
    }

    SECTION("format floating-point value") {
        using value_type = double;
        value_type val = 0.0;
        const auto get_val = [&val] { return val; };
        iteration_logger_item<value_type, decltype(get_val)> item{
            "abc", get_val};
        constexpr num_collect::index_type width = 7;
        constexpr num_collect::index_type precision = 3;
        item.width(width);
        item.precision(precision);
        CHECK(item.width() == width);
        CHECK(item.precision() == precision);

        val = 3.14;  // NOLINT

        fmt::memory_buffer buffer;
        item.format_value_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "   3.14");

        buffer.clear();
        item.format_label_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "    abc");

        buffer.clear();
        item.format_summary_to(buffer);
        CHECK(std::string(buffer.data(), buffer.size()) == "abc=3.14");
    }
}

TEST_CASE("num_collect::logging::iteration_logger") {
    using trompeloeil::_;

    constexpr num_collect::index_type iteration_output_period = 2;
    constexpr num_collect::index_type iteration_label_period = 3;

    constexpr auto tag = num_collect::logging::log_tag_view(
        "num_collect::logging::iteration_logger_test");
    const auto sink =
        std::make_shared<num_collect_test::logging::mock_log_sink>();
    const auto config = num_collect::logging::log_tag_config()
                            .write_traces(true)
                            .write_iterations(true)
                            .write_summary(true)
                            .iteration_output_period(iteration_output_period)
                            .iteration_label_period(iteration_label_period)
                            .sink(sink);
    const auto logger = num_collect::logging::logger(tag, config);
    auto iteration_logger = num_collect::logging::iteration_logger();

    SECTION("set items") {
        constexpr num_collect::index_type width = 7;
        int val1 = 0;
        const auto get_val1 = [&val1] { return val1; };
        iteration_logger.append<int>("val1", get_val1)->width(width);
        double val2 = 0.0;
        iteration_logger.append("val2", val2)->width(width)->precision(3);
        std::string val3;
        iteration_logger.append("val3", val3)->width(width);

        std::vector<std::string> logs;
        REQUIRE_CALL(*sink, write(_, _, _, _, _))
            .TIMES(2)
            // NOLINTNEXTLINE
            .LR_SIDE_EFFECT(logs.emplace_back(_5));

        val1 = 12345;  // NOLINT
        val2 = 3.14;   // NOLINT
        val3 = "abc";
        iteration_logger.write_iteration_to(logger);

        REQUIRE(logs.size() == 2);
        CHECK(logs.at(0) == "    val1    val2    val3");
        CHECK(logs.at(1) == "   12345    3.14     abc");
    }

    SECTION("take period configurations into account") {
        constexpr num_collect::index_type width = 7;
        int val1 = 0;
        iteration_logger.append("val1", val1)->width(width);
        double val2 = 0.0;
        iteration_logger.append("val2", val2)->width(width)->precision(3);
        std::string val3;
        iteration_logger.append("val3", val3)->width(width);

        std::vector<std::string> logs;
        ALLOW_CALL(*sink, write(_, _, _, _, _))
            // NOLINTNEXTLINE
            .LR_SIDE_EFFECT(logs.emplace_back(_5));

        val2 = 3.14;  // NOLINT
        val3 = "abc";

        constexpr int repetition = 10;
        for (int i = 0; i < repetition; ++i) {
            val1 = i;
            iteration_logger.write_iteration_to(logger);
        }

        CHECK(logs.size() == 7);                          // NOLINT
        CHECK(logs.at(0) == "    val1    val2    val3");  // 0th time.
        CHECK(logs.at(1) == "       0    3.14     abc");  // 0th time.
        CHECK(logs.at(2) == "       2    3.14     abc");  // 2nd time.
        CHECK(logs.at(3) == "       4    3.14     abc");  // 4th time.
        CHECK(logs.at(4) == "    val1    val2    val3");  // 6th time.
        CHECK(logs.at(5) == "       6    3.14     abc");  // 6th time.
        CHECK(logs.at(6) == "       8    3.14     abc");  // 8th time.
    }

    SECTION("reset iteration count") {
        constexpr num_collect::index_type width = 7;
        int val1 = 0;
        iteration_logger.append("val1", val1)->width(width);
        double val2 = 0.0;
        iteration_logger.append("val2", val2)->width(width)->precision(3);
        std::string val3;
        iteration_logger.append("val3", val3)->width(width);

        std::vector<std::string> logs;
        ALLOW_CALL(*sink, write(_, _, _, _, _))
            // NOLINTNEXTLINE
            .LR_SIDE_EFFECT(logs.emplace_back(_5));

        val2 = 3.14;  // NOLINT
        val3 = "abc";

        constexpr int repetition = 3;
        for (int i = 0; i < repetition; ++i) {
            iteration_logger.reset_count();
            val1 = i;
            iteration_logger.write_iteration_to(logger);
        }

        CHECK(logs.size() == 6);                          // NOLINT
        CHECK(logs.at(0) == "    val1    val2    val3");  // 0th time.
        CHECK(logs.at(1) == "       0    3.14     abc");  // 0th time.
        CHECK(logs.at(2) == "    val1    val2    val3");  // 1st time.
        CHECK(logs.at(3) == "       1    3.14     abc");  // 1st time.
        CHECK(logs.at(4) == "    val1    val2    val3");  // 2nd time.
        CHECK(logs.at(5) == "       2    3.14     abc");  // 2nd time.
    }

    SECTION("write summary") {
        constexpr num_collect::index_type width = 7;
        int val1 = 0;
        iteration_logger.append("val1", val1)->width(width);
        double val2 = 0.0;
        iteration_logger.append("val2", val2)->width(width)->precision(3);
        std::string val3;
        iteration_logger.append("val3", val3)->width(width);

        std::vector<std::string> logs;
        ALLOW_CALL(*sink, write(_, _, _, _, _))
            // NOLINTNEXTLINE
            .LR_SIDE_EFFECT(logs.emplace_back(_5));

        val1 = 12345;  // NOLINT
        val2 = 3.14;   // NOLINT
        val3 = "abc";

        iteration_logger.write_summary_to(logger);

        CHECK(logs.size() == 1);  // NOLINT
        CHECK(logs.at(0) == "Last state: val1=12345, val2=3.14, val3=abc, ");
    }
}
