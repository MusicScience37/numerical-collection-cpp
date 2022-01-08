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
 * \brief Test of log_tag_config class.
 */
#include "num_collect/logging/log_tag_config.h"

#include <catch2/catch_test_macros.hpp>

#include "mock_log_sink.h"
#include "num_collect/util/index_type.h"

TEST_CASE("num_collect::logging::log_tag_config") {
    using num_collect::logging::log_tag_config;

    SECTION("default config") {
        const auto config = log_tag_config();

        CHECK(config.sink() != nullptr);
        CHECK(!config.write_traces());
        CHECK(!config.write_iterations());
        CHECK(!config.write_summary());
        CHECK(config.iteration_output_period() == 1);
        CHECK(config.iteration_label_period() > 0);
    }

    SECTION("set config") {
        auto config = log_tag_config();

        SECTION("sink") {
            CHECK_THROWS(config.sink(nullptr));
            CHECK(config.sink() != nullptr);

            const auto sink =
                std::make_shared<num_collect_test::logging::mock_log_sink>();
            CHECK(config.sink(sink).sink() == sink);
        }

        SECTION("write_traces") {
            CHECK(config.write_traces(true).write_traces());
        }

        SECTION("write_iterations") {
            CHECK(config.write_iterations(true).write_iterations());
        }

        SECTION("write_summary") {
            CHECK(config.write_summary(true).write_summary());
        }

        SECTION("iteration_output_period") {
            CHECK_THROWS(config.iteration_output_period(0));

            const num_collect::index_type val = 123;
            CHECK(
                config.iteration_output_period(val).iteration_output_period() ==
                val);
        }

        SECTION("iteration_label_period") {
            CHECK_THROWS(config.iteration_label_period(0));

            const num_collect::index_type val = 123;
            CHECK(config.iteration_label_period(val).iteration_label_period() ==
                val);
        }
    }
}
