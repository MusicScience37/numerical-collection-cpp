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
 * \brief Test of combined_log_sink class.
 */
#include "num_collect/logging/sinks/combined_log_sink.h"

#include <string>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "../mock_log_sink.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "trompeloeil_catch2.h"

TEST_CASE("num_collect::logging::sinks::combined_log_sink") {
    using num_collect::logging::log_level;
    using num_collect::logging::sinks::create_combined_log_sink;
    using num_collect::logging::sinks::log_sink_base;
    using num_collect::util::source_info_view;
    using num_collect_test::logging::mock_log_sink;
    using ::trompeloeil::_;

    SECTION("asynchronously write a log") {
        const auto time = std::chrono::system_clock::now();
        const auto tag = std::string("Tag");
        const auto file_path = std::string("filepath");
        const num_collect::index_type line = 123;
        const num_collect::index_type column = 7;
        const auto function_name = std::string("function");
        const auto source =
            source_info_view(file_path, line, column, function_name);
        const auto body = std::string("body");

        const auto inner_sink1 = std::make_shared<mock_log_sink>();
        const auto inner_sink2 = std::make_shared<mock_log_sink>();

        const auto combined_sink = create_combined_log_sink(
            std::vector<std::pair<std::shared_ptr<log_sink_base>, log_level>>{
                {inner_sink1, log_level::info},
                {inner_sink2, log_level::debug},
            });

        {
            FORBID_CALL(*inner_sink1, write_impl(_, _, _, _, _));
            FORBID_CALL(*inner_sink2, write_impl(_, _, _, _, _));
            combined_sink->write(time, tag, log_level::trace, source, body);
        }
        {
            FORBID_CALL(*inner_sink1, write_impl(_, _, _, _, _));
            REQUIRE_CALL(*inner_sink2, write_impl(_, _, _, _, _)).TIMES(1);
            combined_sink->write(time, tag, log_level::debug, source, body);
        }
        {
            FORBID_CALL(*inner_sink1, write_impl(_, _, _, _, _));
            REQUIRE_CALL(*inner_sink2, write_impl(_, _, _, _, _)).TIMES(1);
            combined_sink->write(time, tag, log_level::summary, source, body);
        }
        {
            REQUIRE_CALL(*inner_sink1, write_impl(_, _, _, _, _)).TIMES(1);
            REQUIRE_CALL(*inner_sink2, write_impl(_, _, _, _, _)).TIMES(1);
            combined_sink->write(time, tag, log_level::info, source, body);
        }
        {
            REQUIRE_CALL(*inner_sink1, write_impl(_, _, _, _, _)).TIMES(1);
            REQUIRE_CALL(*inner_sink2, write_impl(_, _, _, _, _)).TIMES(1);
            combined_sink->write(time, tag, log_level::warning, source, body);
        }
    }
}
