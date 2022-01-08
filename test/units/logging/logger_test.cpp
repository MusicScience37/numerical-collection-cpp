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
 * \brief Definition of logger class.
 */
#include "num_collect/logging/logger.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "mock_log_sink.h"

TEST_CASE("num_collect::logging::logger") {
    using num_collect::logging::log_config;
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag_config;
    using num_collect::logging::log_tag_view;
    using num_collect::logging::logger;
    using num_collect_test::logging::mock_log_sink;
    using ::trompeloeil::_;

    SECTION("construct") {
        constexpr auto tag = log_tag_view("num_collect::logging::logger_test1");
        const num_collect::index_type iteration_label_period = 123;
        const auto config =
            log_tag_config().iteration_label_period(iteration_label_period);
        CHECK_NOTHROW(log_config::instance().set_config_of(tag, config));

        const auto l = logger(tag);
        CHECK(l.config().iteration_label_period() == iteration_label_period);
    }

    SECTION("write logs with various log levels") {
        constexpr auto tag = log_tag_view("num_collect::logging::logger_test2");
        const auto sink = std::make_shared<mock_log_sink>();
        const auto config = log_tag_config()
                                .write_traces(true)
                                .write_iterations(true)
                                .write_summary(true)
                                .sink(sink);
        CHECK_NOTHROW(log_config::instance().set_config_of(tag, config));

        REQUIRE_CALL(*sink, write(_, _, _, _, _)).TIMES(7);

        const auto l = logger(tag);
        l.trace()("Test trace log.");
        l.iteration()("Test iteration log.");
        l.iteration_label()("Test iteration_label log.");
        l.summary()("Test summary log.");
        l.info()("Test info log.");
        l.warning()("Test warning log.");
        l.error()("Test error log.");
    }

    SECTION("write logs with formatting") {
        constexpr auto tag = log_tag_view("num_collect::logging::logger_test3");
        const auto sink = std::make_shared<mock_log_sink>();
        const auto config = log_tag_config().sink(sink);
        CHECK_NOTHROW(log_config::instance().set_config_of(tag, config));

        REQUIRE_CALL(*sink,
            write(_, std::string_view(tag.name()), log_level::info, _,
                std::string_view("Test info log with formatting. (value=3)")));

        const auto l = logger(tag);
        l.info()("Test info log {}. (value={})", "with formatting", 3);
    }
}
