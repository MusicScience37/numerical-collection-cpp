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

#include "num_collect/util/index_type.h"

TEST_CASE("num_collect::logging::iteration_logger_item") {
    using num_collect::logging::iteration_logger_item;

    SECTION("format integers") {
        using value_type = num_collect::index_type;
        value_type val = 0;
        iteration_logger_item<value_type> item{"abc", val};
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
    }

    SECTION("format string") {
        using value_type = std::string;
        value_type val;
        iteration_logger_item<value_type> item{"abc", val};
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
    }

    SECTION("format floating-point value") {
        using value_type = double;
        value_type val = 0.0;
        iteration_logger_item<value_type> item{"abc", val};
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
    }
}
