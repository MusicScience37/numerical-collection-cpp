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
 * \brief Test of iteration_parameter class.
 */
#include "num_collect/logging/iterations/iteration_parameter.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "mock_algorithm.h"
#include "num_collect/logging/iterations/member_function_iteration_parameter_value.h"
#include "num_collect/logging/iterations/member_variable_iteration_parameter_value.h"
#include "num_collect/logging/iterations/variable_iteration_parameter_value.h"

TEST_CASE("num_collect::logging::iterations::iteration_parameter") {
    using num_collect::logging::iterations::iteration_parameter;
    using num_collect::logging::iterations::
        member_function_iteration_parameter_value;
    using num_collect::logging::iterations::
        member_variable_iteration_parameter_value;
    using num_collect::logging::iterations::variable_iteration_parameter_value;
    using num_collect_test::logging::iterations::mock_algorithm;

    SECTION("use variable_iteration_parameter_value") {
        using algorithm_type = void;
        using value_type = int;
        using parameter_value_type =
            variable_iteration_parameter_value<algorithm_type, value_type>;
        using parameter_type = iteration_parameter<algorithm_type, value_type,
            parameter_value_type>;

        const std::string label = "abc";
        value_type value = 12345;  // NOLINT
        constexpr num_collect::index_type width = 7;
        parameter_type parameter{label, parameter_value_type{value}};
        parameter.width(width);

        CHECK(parameter.label() == label);
        CHECK(parameter.width() == width);

        fmt::memory_buffer buffer;
        CHECK_NOTHROW(parameter.format_label_to(buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "    abc");

        buffer.clear();
        CHECK_NOTHROW(parameter.format_value_to(buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "  12345");

        buffer.clear();
        CHECK_NOTHROW(parameter.format_summary_to(buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "abc=12345");

        void* algorithm = nullptr;
        buffer.clear();
        CHECK_NOTHROW(parameter.format_value_to(buffer, algorithm));
        CHECK(std::string(buffer.data(), buffer.size()) == "  12345");

        buffer.clear();
        CHECK_NOTHROW(parameter.format_summary_to(buffer, algorithm));
        CHECK(std::string(buffer.data(), buffer.size()) == "abc=12345");
    }

    SECTION("use member_variable_iteration_parameter_value") {
        using value_type = int;
        using algorithm_type = mock_algorithm<value_type, value_type>;
        using parameter_value_type =
            member_variable_iteration_parameter_value<algorithm_type,
                value_type>;
        using parameter_type = iteration_parameter<algorithm_type, value_type,
            parameter_value_type>;

        const std::string label = "abc";
        algorithm_type algorithm{12345};  // NOLINT
        constexpr num_collect::index_type width = 7;
        parameter_type parameter{
            label, parameter_value_type{&algorithm_type::value1}};
        parameter.width(width);

        CHECK(parameter.label() == label);
        CHECK(parameter.width() == width);

        fmt::memory_buffer buffer;
        CHECK_NOTHROW(parameter.format_label_to(buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "    abc");

        buffer.clear();
        CHECK_THROWS(parameter.format_value_to(buffer));

        buffer.clear();
        CHECK_THROWS(parameter.format_summary_to(buffer));

        buffer.clear();
        CHECK_NOTHROW(parameter.format_value_to(buffer, &algorithm));
        CHECK(std::string(buffer.data(), buffer.size()) == "  12345");

        buffer.clear();
        CHECK_NOTHROW(parameter.format_summary_to(buffer, &algorithm));
        CHECK(std::string(buffer.data(), buffer.size()) == "abc=12345");
    }

    SECTION("use member_function_iteration_parameter_value") {
        using value_type = int;
        using return_type = value_type;
        using algorithm_type = mock_algorithm<value_type, return_type>;
        using parameter_value_type =
            member_function_iteration_parameter_value<algorithm_type,
                value_type, return_type>;
        using parameter_type = iteration_parameter<algorithm_type, value_type,
            parameter_value_type>;

        const std::string label = "abc";
        value_type value = 12345;     // NOLINT
        algorithm_type algorithm{0};  // NOLINT
        // NOLINTNEXTLINE
        ALLOW_CALL(algorithm, get_impl()).RETURN(value);
        constexpr num_collect::index_type width = 7;
        parameter_type parameter{
            label, parameter_value_type{&algorithm_type::get}};
        parameter.width(width);

        CHECK(parameter.label() == label);
        CHECK(parameter.width() == width);

        fmt::memory_buffer buffer;
        CHECK_NOTHROW(parameter.format_label_to(buffer));
        CHECK(std::string(buffer.data(), buffer.size()) == "    abc");

        buffer.clear();
        CHECK_THROWS(parameter.format_value_to(buffer));

        buffer.clear();
        CHECK_THROWS(parameter.format_summary_to(buffer));

        buffer.clear();
        CHECK_NOTHROW(parameter.format_value_to(buffer, &algorithm));
        CHECK(std::string(buffer.data(), buffer.size()) == "  12345");

        buffer.clear();
        CHECK_NOTHROW(parameter.format_summary_to(buffer, &algorithm));
        CHECK(std::string(buffer.data(), buffer.size()) == "abc=12345");
    }
}
