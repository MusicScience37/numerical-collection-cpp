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
 * \brief Test of iteration_parameter_formatter class.
 */
#include "num_collect/logging/iterations/iteration_parameter_formatter.h"

#include <optional>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/concepts/formattable_iteration_parameter_value.h"  // IWYU pragma: keep

TEST_CASE("num_collect::logging::iterations::iteration_parameter_formatter") {
    using num_collect::logging::concepts::formattable_iteration_parameter_value;
    using num_collect::logging::iterations::iteration_parameter_formatter;

    SECTION("int") {
        STATIC_REQUIRE(formattable_iteration_parameter_value<int>);

        iteration_parameter_formatter<int> formatter;
        constexpr int val1 = 12345;
        fmt::memory_buffer buffer;
        CHECK_NOTHROW(formatter.format(val1, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "12345");

        constexpr int val2 = -234;
        constexpr num_collect::index_type width = 7;
        buffer.clear();
        CHECK_NOTHROW(formatter.format_with_alignment(val2, width, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "   -234");
    }

    SECTION("std::string") {
        STATIC_REQUIRE(formattable_iteration_parameter_value<std::string>);

        iteration_parameter_formatter<std::string> formatter;
        const std::string val1 = "abc";
        fmt::memory_buffer buffer;
        CHECK_NOTHROW(formatter.format(val1, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "abc");

        const std::string val2 = "de";
        constexpr num_collect::index_type width = 3;
        buffer.clear();
        CHECK_NOTHROW(formatter.format_with_alignment(val2, width, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == " de");
    }

    SECTION("double") {
        STATIC_REQUIRE(formattable_iteration_parameter_value<double>);

        iteration_parameter_formatter<double> formatter;
        constexpr double val1 = 1.234;
        fmt::memory_buffer buffer;
        CHECK_NOTHROW(formatter.format(val1, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "1.234");

        constexpr double val2 = -2.345;
        constexpr num_collect::index_type width = 7;
        buffer.clear();
        CHECK_NOTHROW(formatter.format_with_alignment(val2, width, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == " -2.345");

        constexpr double val3 = 3.141592;
        constexpr num_collect::index_type precision = 6;
        formatter.precision(precision);
        buffer.clear();
        CHECK_NOTHROW(formatter.format(val3, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "3.14159");

        CHECK_THROWS(formatter.precision(-1));
        CHECK_THROWS(formatter.precision(0));
        CHECK_NOTHROW(formatter.precision(1));
    }

    SECTION("optional<int>") {
        STATIC_REQUIRE(
            formattable_iteration_parameter_value<std::optional<int>>);

        iteration_parameter_formatter<std::optional<int>> formatter;
        const std::optional<int> val1 = 12345;
        fmt::memory_buffer buffer;
        CHECK_NOTHROW(formatter.format(val1, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "12345");

        const std::optional<int> val2 = -234;
        constexpr num_collect::index_type width = 7;
        buffer.clear();
        CHECK_NOTHROW(formatter.format_with_alignment(val2, width, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "   -234");

        const std::optional<int> null_val{};
        buffer.clear();
        CHECK_NOTHROW(formatter.format(null_val, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "null");

        buffer.clear();
        CHECK_NOTHROW(formatter.format_with_alignment(null_val, width, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "   null");
    }

    SECTION("optional<double>") {
        STATIC_REQUIRE(
            formattable_iteration_parameter_value<std::optional<double>>);

        iteration_parameter_formatter<std::optional<double>> formatter;
        const std::optional<double> val1 = 1.234;
        fmt::memory_buffer buffer;
        CHECK_NOTHROW(formatter.format(val1, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "1.234");

        const std::optional<double> val2 = -2.345;
        constexpr num_collect::index_type width = 7;
        buffer.clear();
        CHECK_NOTHROW(formatter.format_with_alignment(val2, width, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == " -2.345");

        const std::optional<double> val3 = 3.141592;
        constexpr num_collect::index_type precision = 6;
        formatter.precision(precision);
        buffer.clear();
        CHECK_NOTHROW(formatter.format(val3, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "3.14159");

        const std::optional<double> val4{};
        buffer.clear();
        CHECK_NOTHROW(formatter.format(val4, buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "null");

        CHECK_THROWS(formatter.precision(-1));
        CHECK_THROWS(formatter.precision(0));
        CHECK_NOTHROW(formatter.precision(1));
    }
}
