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

#include <string>

#include <ApprovalTests.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "../mock_log_sink.h"
#include "mock_algorithm.h"
#include "num_collect/logging/log_tag_view.h"

TEST_CASE("num_collect::logging::iterations::iteration_logger") {
    using num_collect::logging::iterations::iteration_logger;
    using num_collect_test::logging::iterations::mock_algorithm;
    using ::trompeloeil::_;

    constexpr num_collect::index_type iteration_output_period = 2;
    constexpr num_collect::index_type iteration_label_period = 3;

    constexpr auto tag = num_collect::logging::log_tag_view(
        "num_collect::logging::iterations::iteration_logger_test");
    const auto sink =
        std::make_shared<num_collect_test::logging::mock_log_sink>();
    const auto config =
        num_collect::logging::log_tag_config()
            .output_log_level(num_collect::logging::log_level::trace)
            .output_log_level_in_child_iterations(
                num_collect::logging::log_level::summary)
            .iteration_output_period(iteration_output_period)
            .iteration_label_period(iteration_label_period)
            .sink(sink);
    auto logger = num_collect::logging::logger(tag, config);

    fmt::memory_buffer logs;
    ALLOW_CALL(*sink, write_impl(_, _, _, _, _))
        // NOLINTNEXTLINE
        .LR_SIDE_EFFECT(logs.append(static_cast<std::string>(_5) + '\n'));

    SECTION("write iterations without an algorithm") {
        using value_type = int;
        using iteration_logger_type = iteration_logger<>;

        iteration_logger_type iter_logger{logger};

        value_type value{0};

        iter_logger.append<value_type>("value", value);

        iter_logger.start(logger);

        constexpr int repetition = 20;
        for (int i = 0; i < repetition; ++i) {
            value = i;  // NOLINT(clang-analyzer-deadcode.DeadStores)
            iter_logger.write_iteration();
        }
        iter_logger.write_summary();

        ApprovalTests::Approvals::verify(std::string(logs.data(), logs.size()));
    }

    SECTION("write iterations of algorithms") {
        using value1_type = int;
        using value2_type = std::string;
        using value3_type = double;
        using return2_type = const value2_type&;
        using algorithm_type = mock_algorithm<value1_type, return2_type>;
        using iteration_logger_type = iteration_logger<algorithm_type>;

        iteration_logger_type iter_logger{logger};

        algorithm_type algorithm{0};
        value2_type value2{"abc"};
        // NOLINTNEXTLINE
        ALLOW_CALL(algorithm, get_impl()).RETURN(value2);
        value3_type value3{1.234};  // NOLINT

        iter_logger.append<value1_type>("value1", &algorithm_type::value1);
        iter_logger.append<value2_type>("value2", &algorithm_type::get);
        iter_logger.append<value3_type>("value3", value3)
            ->formatter()
            .precision(3);

        iter_logger.start(logger);

        constexpr int repetition = 20;
        for (int i = 0; i < repetition; ++i) {
            algorithm.value1 = i;
            iter_logger.write_iteration(&algorithm);
        }
        iter_logger.write_summary(&algorithm);

        ApprovalTests::Approvals::verify(std::string(logs.data(), logs.size()));
    }
}
