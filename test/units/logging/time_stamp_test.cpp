/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of time_stamp class.
 */
#include "num_collect/logging/time_stamp.h"

#include <chrono>
#include <compare>
#include <cstdint>
#include <ctime>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

TEST_CASE("num_collect::logging::time_stamp") {
    using num_collect::logging::time_stamp;

    SECTION("construct a time stamp") {
        constexpr std::time_t seconds = 1703334965;
        constexpr std::uint32_t nanoseconds = 123456789;

        time_stamp stamp(seconds, nanoseconds);

        CHECK(stamp.seconds() == seconds);
        CHECK(stamp.nanoseconds() == nanoseconds);
    }

    SECTION("get the current time stamp") {
        const auto time_point_before = std::chrono::system_clock::now();
        const time_stamp stamp = time_stamp::now();
        const auto time_point_after = std::chrono::system_clock::now();

        const auto stamp_as_time_point = std::chrono::time_point_cast<
            std::chrono::system_clock::time_point::duration>(
            std::chrono::system_clock::from_time_t(stamp.seconds()) +
            std::chrono::nanoseconds(stamp.nanoseconds()));
        CHECK(time_point_before <= stamp_as_time_point);
        CHECK(stamp_as_time_point <= time_point_after);
    }

    SECTION("format a time stamp") {
        constexpr std::time_t seconds = 1703334965;
        constexpr std::uint32_t nanoseconds = 123456789;
        time_stamp stamp(seconds, nanoseconds);

        const auto formatted_stamp = fmt::format("{}", stamp);

        CHECK(formatted_stamp == "2023-12-23T12:36:05.123456789");
    }
}
