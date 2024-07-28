/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of macros for logging.
 */
#include "num_collect/logging/logging_macros.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "mock_log_sink.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "trompeloeil_catch2.h"

TEST_CASE("NUM_COLLECT_LOG_AND_THROW") {
    using ::trompeloeil::_;

    const auto tag = num_collect::logging::log_tag_view(
        "num_collect_test::logging::logging_macros");
    const auto sink =
        std::make_shared<num_collect_test::logging::mock_log_sink>();
    const auto config =
        num_collect::logging::log_tag_config().sink(sink->to_log_sink());
    const auto logger = num_collect::logging::logger(tag, config);

    SECTION(
        "write log and throw exception with a logger and with a parameter") {
        REQUIRE_CALL(*sink, write_impl(_, _, _, _, _)).TIMES(1);

        try {
            NUM_COLLECT_LOG_AND_THROW(num_collect::invalid_argument, logger,
                "Test exception with parameter: {}", 12345);
            FAIL();
        } catch (const num_collect::invalid_argument& exception) {
            CHECK_THAT(exception.what(),
                Catch::Matchers::ContainsSubstring(
                    "Test exception with parameter: 12345"));
        }
    }

    SECTION(
        "write log and throw exception with a logger and without a parameter") {
        REQUIRE_CALL(*sink, write_impl(_, _, _, _, _)).TIMES(1);

        try {
            NUM_COLLECT_LOG_AND_THROW(
                num_collect::invalid_argument, logger, "Test exception.");
            FAIL();
        } catch (const num_collect::invalid_argument& exception) {
            CHECK_THAT(exception.what(),
                Catch::Matchers::ContainsSubstring("Test exception."));
        }
    }

    SECTION(
        "write log and throw exception without a logger and with a parameter") {
        try {
            NUM_COLLECT_LOG_AND_THROW(num_collect::invalid_argument,
                "Test exception with parameter: {}", 12345);
            FAIL();
        } catch (const num_collect::invalid_argument& exception) {
            CHECK_THAT(exception.what(),
                Catch::Matchers::ContainsSubstring(
                    "Test exception with parameter: 12345"));
        }
    }

    SECTION(
        "write log and throw exception without a logger and without a "
        "parameter") {
        try {
            NUM_COLLECT_LOG_AND_THROW(
                num_collect::invalid_argument, "Test exception.");
            FAIL();
        } catch (const num_collect::invalid_argument& exception) {
            CHECK_THAT(exception.what(),
                Catch::Matchers::ContainsSubstring("Test exception."));
        }
    }
}
