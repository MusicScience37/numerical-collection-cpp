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
 * \brief Test of iso8601_time class.
 */
#include "num_collect/logging/formatters/iso8601_time.h"

#include <chrono>
#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::formatters::iso8601_time") {
    using num_collect::logging::formatters::iso8601_time;

    SECTION("format") {
        // 2022/8/28 17:56:14.123456
        const auto time_since_epoch = std::chrono::seconds{1661709374} +
            std::chrono::microseconds{123456};
        const auto time_point =
            std::chrono::system_clock::time_point{time_since_epoch};
        const auto time_for_format = iso8601_time{time_point};
        CHECK((fmt::format("{}", time_for_format)) ==
            "2022-08-28T17:56:14.123456");
    }
}
