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
 * \brief Definition of catch_event_listener class.
 */
// clang-format off
// These header must be included in this order.
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/catch_reporter_registrars.hpp>
// clang-format on

#include <memory>
#include <optional>

#include <catch2/catch_test_case_info.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <fmt/format.h>

#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/simple_log_sink.h"

#define STRING1(STR) #STR
#define STRING(STR) STRING1(STR)

class catch_event_listener : public Catch::EventListenerBase {
public:
    explicit catch_event_listener(const Catch::ReporterConfig& config)
        : EventListenerBase(config) {}

    void testRunStarting(const Catch::TestRunInfo& /*testRunInfo*/) override {
        const auto file_path = fmt::format("num_collect_test_units_{}.log",
            STRING(NUM_COLLECT_TEST_MODULE_NAME));
        const auto sink =
            std::make_shared<num_collect::logging::simple_log_sink>(file_path);
        const auto config = num_collect::logging::log_tag_config()
                                .sink(sink)
                                .write_traces(true)
                                .write_iterations(true)
                                .write_summary(true);
        num_collect::logging::log_config::instance().set_default_tag_config(
            config);

        logger_ = num_collect::logging::logger();
        logger_.value().info()(std::string(line_length, '='));
        logger_.value().info()("Start test.");
    }

    void testCaseStarting(const Catch::TestCaseInfo& test_info) override {
        logger_.value().info()(std::string(line_length, '='));
        logger_.value().info()("Start test case {}.", test_info.name);
    }

    void sectionStarting(const Catch::SectionInfo& section_info) override {
        logger_.value().info()(std::string(line_length, '-'));
        logger_.value().info()("Start test section {}.", section_info.name);
    }

    void sectionEnded(const Catch::SectionStats& section_stats) override {
        logger_.value().info()(
            "Finished test section {}.", section_stats.sectionInfo.name);
        logger_.value().info()("Passed {} assertions, failed {} assertions.",
            section_stats.assertions.passed, section_stats.assertions.failed);
    }

    void testRunEnded(const Catch::TestRunStats& test_run_stats) override {
        logger_.value().info()(std::string(line_length, '='));
        logger_.value().info()("Finished send_data_test_units.");
        logger_.value().info()("Passed {} tests, failed {} tests.",
            test_run_stats.totals.testCases.passed,
            test_run_stats.totals.testCases.failed);
    }

private:
    static constexpr std::size_t line_length = 128;

    std::optional<num_collect::logging::logger> logger_{};
};
// NOLINTNEXTLINE
CATCH_REGISTER_LISTENER(catch_event_listener);
