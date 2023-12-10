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
 * \brief Test of async_log_sink class.
 */
#include <chrono>
#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "../mock_log_sink.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/async_logging_worker_config.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/logging/sinks/log_sinks.h"
#include "num_collect/util/source_info_view.h"
#include "trompeloeil_catch2.h"

TEST_CASE("num_collect::logging::sinks::async_log_sink") {
    using num_collect::logging::log_level;
    using num_collect::logging::sinks::async_logging_worker_config;
    using num_collect::logging::sinks::create_async_log_sink;
    using num_collect::logging::sinks::log_sink_base;
    using num_collect::util::source_info_view;
    using num_collect_test::logging::mock_log_sink;
    using ::trompeloeil::_;

    struct async_log_request {
    public:
        //! Time.
        std::chrono::system_clock::time_point time;

        //! Tag.
        std::string tag;

        //! Log level.
        log_level level;

        //! Filepath.
        std::string file_path;

        //! Line number.
        num_collect::index_type line;

        //! Column number.
        num_collect::index_type column;

        //! Function name.
        std::string function_name;

        //! Log body.
        std::string body;

        //! Log sink to write to.
        std::shared_ptr<log_sink_base> sink;
    };

    SECTION("asynchronously write a log") {
        const auto inner_sink = std::make_shared<mock_log_sink>();

        std::promise<async_log_request> request_promise;
        auto request_future = request_promise.get_future();
        ALLOW_CALL(*inner_sink, write_impl(_, _, _, _, _))
            // NOLINTNEXTLINE(readability-identifier-naming)
            .LR_SIDE_EFFECT(
                (request_promise.set_value(async_log_request{.time = _1,
                    .tag = std::string{_2},
                    .level = _3,
                    .file_path = std::string{_4.file_path()},
                    .line = _4.line(),
                    .column = _4.column(),
                    .function_name = std::string{_4.function_name()},
                    .body = std::string{_5}})));

        const auto time = std::chrono::system_clock::now();
        const auto tag = std::string("Tag");
        const auto level = log_level::summary;
        const auto file_path = std::string("filepath");
        const num_collect::index_type line = 123;
        const num_collect::index_type column = 7;
        const auto function_name = std::string("function");
        const auto source =
            source_info_view(file_path, line, column, function_name);
        const auto body = std::string("body");

        const auto async_sink = create_async_log_sink(inner_sink);
        async_sink->write(time, tag, level, source, body);

        REQUIRE(request_future.wait_for(std::chrono::seconds(1)) ==
            std::future_status::ready);
        const auto request = request_future.get();
        CHECK(request.time == time);
        CHECK(request.tag == tag);
        CHECK(request.level == level);
        CHECK(request.file_path == file_path);
        CHECK(request.line == line);
        CHECK(request.column == column);
        CHECK(request.function_name == function_name);
        CHECK(request.body == body);
    }
}
