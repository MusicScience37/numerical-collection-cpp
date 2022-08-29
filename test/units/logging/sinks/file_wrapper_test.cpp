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
 * \brief Test of file_wrapper class.
 */
#include "num_collect/logging/sinks/file_wrapper.h"

#include <fstream>
#include <iterator>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::sinks::file_wrapper") {
    using num_collect::logging::sinks::file_wrapper;

    SECTION("default constructor") {
        file_wrapper file{};
        CHECK(static_cast<void*>(file.file()) == nullptr);
        CHECK_THROWS(file.write("data"));
    }

    SECTION("initialize with a file") {
        file_wrapper file{stdout, false};
        CHECK(static_cast<void*>(file.file()) == static_cast<void*>(stdout));
    }

    SECTION("write to a file") {
        const std::string filepath =
            "num_collect_test_units_logging_file_wrapper_text.txt";
        const std::string data = "Test data 1.";
        {
            file_wrapper file{filepath, "w"};
            CHECK_NOTHROW(file.write(data));
        }
        {
            std::ifstream stream{filepath};
            const auto actual_data =
                std::string(std::istreambuf_iterator<char>(stream),
                    std::istreambuf_iterator<char>());
            CHECK(actual_data == data);
        }
    }

    SECTION("move constructor") {
        const std::string filepath =
            "num_collect_test_units_logging_file_wrapper_text.txt";
        const std::string data = "Test data 2.";
        {
            file_wrapper origin{filepath, "w"};
            file_wrapper file{std::move(origin)};
            CHECK_NOTHROW(file.write(data));
        }
        {
            std::ifstream stream{filepath};
            const auto actual_data =
                std::string(std::istreambuf_iterator<char>(stream),
                    std::istreambuf_iterator<char>());
            CHECK(actual_data == data);
        }
    }

    SECTION("move assignment operator") {
        const std::string filepath =
            "num_collect_test_units_logging_file_wrapper_text.txt";
        const std::string data = "Test data 3.";
        {
            file_wrapper file{};
            file = file_wrapper{filepath, "w"};
            CHECK_NOTHROW(file.write(data));
        }
        {
            std::ifstream stream{filepath};
            const auto actual_data =
                std::string(std::istreambuf_iterator<char>(stream),
                    std::istreambuf_iterator<char>());
            CHECK(actual_data == data);
        }
    }

    SECTION("set to stdout") {
        file_wrapper file{};
        file.set_stdout();
        CHECK(static_cast<void*>(file.file()) == static_cast<void*>(stdout));
    }

    SECTION("set to stderr") {
        file_wrapper file{};
        file.set_stderr();
        CHECK(static_cast<void*>(file.file()) == static_cast<void*>(stderr));
    }

    SECTION("failure to write") {
        file_wrapper file{stdin, false};
        CHECK(static_cast<void*>(file.file()) == static_cast<void*>(stdin));
        CHECK_THROWS(file.write("data"));
    }
}
