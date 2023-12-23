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
 * \brief Test of iteration_logger class.
 */
#include "num_collect/logging/iterations/iteration_logger.h"

#include <optional>
#include <string>

#include <ApprovalTests.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "../mock_log_sink.h"
#include "mock_algorithm.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_parameter_formatter.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "trompeloeil_catch2.h"

TEST_CASE("num_collect::logging::iterations::iteration_logger") {
    using num_collect::logging::log_level;
    using num_collect::logging::log_tag_config;
    using num_collect::logging::log_tag_view;
    using num_collect::logging::iterations::iteration_logger;
    using num_collect_test::logging::mock_log_sink;
    using num_collect_test::logging::iterations::mock_algorithm;
    using ::trompeloeil::_;

    constexpr num_collect::index_type iteration_output_period = 2;
    constexpr num_collect::index_type iteration_label_period = 3;

    constexpr auto tag =
        log_tag_view("num_collect::logging::iterations::iteration_logger_test");
    const auto sink = std::make_shared<mock_log_sink>();
    const auto config = log_tag_config()
                            .output_log_level(log_level::trace)
                            .iteration_output_period(iteration_output_period)
                            .iteration_label_period(iteration_label_period)
                            .sink(sink->to_log_sink());
    auto logger = num_collect::logging::logger(tag, config);

    fmt::memory_buffer logs;
    ALLOW_CALL(*sink, write_impl(_, _, _, _, _))
        // NOLINTNEXTLINE
        .LR_SIDE_EFFECT(logs.append(static_cast<std::string>(_5) + '\n'));

    SECTION("write iterations without an algorithm") {
        using value1_type = int;
        using iteration_logger_type = iteration_logger<>;

        iteration_logger_type iter_logger{logger};

        value1_type value1{0};
        auto value2_func = []() -> double { return 1.234; };  // NOLINT

        iter_logger.append<value1_type>("value1", value1);
        iter_logger.append<double>("value2", value2_func);

        iter_logger.start(logger);

        constexpr int repetition = 20;
        for (int i = 0; i < repetition; ++i) {
            value1 = i;  // NOLINT(clang-analyzer-deadcode.DeadStores)
            iter_logger.write_iteration();
        }
        iter_logger.write_summary();

        ApprovalTests::Approvals::verify(std::string(logs.data(), logs.size()));
    }

    SECTION("write iterations of algorithms") {
        using value1_type = int;
        using value2_type = std::string;
        using value3_type = double;
        using value4_type = std::optional<int>;
        using return2_type = const value2_type&;
        using algorithm_type = mock_algorithm<value1_type, return2_type>;
        using iteration_logger_type = iteration_logger<algorithm_type>;

        iteration_logger_type iter_logger{logger};

        algorithm_type algorithm{0};
        value2_type value2{"abc"};
        // NOLINTNEXTLINE
        ALLOW_CALL(algorithm, get_impl()).RETURN(value2);
        value3_type value3{1.234};  // NOLINT
        auto value4_func = []() -> value4_type { return std::nullopt; };

        iter_logger.append<value1_type>("value1", &algorithm_type::value1);
        iter_logger.append<value2_type>("value2", &algorithm_type::get);
        iter_logger.append<value3_type>("value3", value3)
            ->formatter()
            .precision(3);
        iter_logger.append<value4_type>("value4", value4_func);

        iter_logger.start(logger);

        constexpr int repetition = 20;
        for (int i = 0; i < repetition; ++i) {
            algorithm.value1 = i;
            iter_logger.write_iteration(&algorithm);
        }
        iter_logger.write_summary(&algorithm);

        ApprovalTests::Approvals::verify(std::string(logs.data(), logs.size()));
    }

    SECTION("write only a summary") {
        using value1_type = int;
        using iteration_logger_type = iteration_logger<>;

        logger = num_collect::logging::logger(
            tag, log_tag_config{config}.output_log_level(log_level::summary));
        iteration_logger_type iter_logger{logger};

        value1_type value1{0};
        auto value2_func = []() -> double { return 1.234; };  // NOLINT

        iter_logger.append<value1_type>("value1", value1);
        iter_logger.append<double>("value2", value2_func);

        iter_logger.start(logger);

        constexpr int repetition = 20;
        for (int i = 0; i < repetition; ++i) {
            value1 = i;  // NOLINT(clang-analyzer-deadcode.DeadStores)
            iter_logger.write_iteration();
        }
        iter_logger.write_summary();

        ApprovalTests::Approvals::verify(std::string(logs.data(), logs.size()));
    }

    SECTION("write no log") {
        using value1_type = int;
        using iteration_logger_type = iteration_logger<>;

        logger = num_collect::logging::logger(
            tag, log_tag_config{config}.output_log_level(log_level::info));
        iteration_logger_type iter_logger{logger};

        value1_type value1{0};
        auto value2_func = []() -> double { return 1.234; };  // NOLINT

        iter_logger.append<value1_type>("value1", value1);
        iter_logger.append<double>("value2", value2_func);

        iter_logger.start(logger);

        constexpr int repetition = 20;
        for (int i = 0; i < repetition; ++i) {
            value1 = i;  // NOLINT(clang-analyzer-deadcode.DeadStores)
            iter_logger.write_iteration();
        }
        iter_logger.write_summary();

        ApprovalTests::Approvals::verify(std::string(logs.data(), logs.size()));
    }

    SECTION("reuse") {
        using value1_type = int;
        using iteration_logger_type = iteration_logger<>;

        iteration_logger_type iter_logger{logger};

        value1_type value1{0};
        auto value2_func = []() -> double { return 1.234; };  // NOLINT

        iter_logger.append<value1_type>("value1", value1);
        iter_logger.append<double>("value2", value2_func);

        constexpr int reuse = 3;
        for (int j = 0; j < reuse; ++j) {
            iter_logger.start(logger);

            constexpr int repetition = 20;
            for (int i = 0; i < repetition; ++i) {
                value1 = i;  // NOLINT(clang-analyzer-deadcode.DeadStores)
                iter_logger.write_iteration();
            }
            iter_logger.write_summary();
        }

        ApprovalTests::Approvals::verify(std::string(logs.data(), logs.size()));
    }
}
