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
 * \brief Test of simple_log_sink class.
 */
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/log_sinks.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"

TEST_CASE("num_collect::logging::sinks::create_single_file_sink") {
    using num_collect::index_type;
    using num_collect::logging::log_level;
    using num_collect::logging::time_stamp;
    using num_collect::logging::sinks::create_single_file_sink;
    using num_collect::util::source_info_view;

    const std::string dir_path = "./logging";
    const std::string filepath = "./logging/simple_log_sink_test.log";
    std::filesystem::remove_all(dir_path);

    const auto sink = create_single_file_sink(filepath);

    SECTION("write") {
        const auto time = time_stamp::now();
        const auto tag = std::string_view("Tag");
        const auto level = log_level::summary;
        const auto file_path = std::string_view("filepath");
        const index_type line = 123;
        const index_type column = 7;
        const auto function_name = std::string_view("function");
        const auto source =
            source_info_view(file_path, line, column, function_name);
        const auto body = std::string_view("body");

        REQUIRE_NOTHROW(sink.write(time, tag, level, source, body));
    }
}
