/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of asynchronous log sinks.
 */
#include "num_collect/logging/sinks/async_log_sink.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "../mock_log_sink.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/log_sink.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"
#include "trompeloeil_catch2.h"

TEST_CASE("num_collect::logging::sinks::async_log_sink") {
    using num_collect::logging::log_level;
    using num_collect::logging::time_stamp;
    using num_collect::logging::sinks::create_async_log_sink;
    using num_collect::logging::sinks::log_sink;
    using num_collect::util::source_info_view;
    using num_collect_test::logging::mock_log_sink;
    using ::trompeloeil::_;

    SECTION("write a log") {
        const auto time = time_stamp::now();
        const auto tag = std::string("Tag");
        const auto file_path = std::string("filepath");
        const num_collect::index_type line = 123;
        const num_collect::index_type column = 7;
        const auto function_name = std::string("function");
        const auto source =
            source_info_view(file_path, line, column, function_name);
        const auto body = std::string("body");

        mock_log_sink mock_actual_sink;

        // Check of the content of the log will be done in another test.
        REQUIRE_CALL(mock_actual_sink, write_impl(_, _, _, _, _)).TIMES(1);
        {
            // Log will be written asynchronously until destruction of
            // async_sink. So, create the sink in a separate scope.
            const auto async_sink =
                create_async_log_sink(mock_actual_sink.to_log_sink());

            async_sink.write(time, tag, log_level::info, source, body);
        }
    }
}
