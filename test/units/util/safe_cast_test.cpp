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
 * \brief Test of safe_cast function.
 */
#include "num_collect/util/safe_cast.h"

#include <cstdint>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

// NOLINTNEXTLINE: I want to use line numbers in test cases.
#define TEST_SAFE(FROM, TO, VALUE)                                        \
    [] {                                                                  \
        using from_type = FROM;                                           \
        using to_type = TO;                                               \
        const auto value = static_cast<from_type>(VALUE);                 \
        to_type casted;                                                   \
        REQUIRE_NOTHROW(casted = num_collect::safe_cast<to_type>(value)); \
        REQUIRE(static_cast<from_type>(casted) == value);                 \
    }()

// NOLINTNEXTLINE: I want to use line numbers in test cases.
#define TEST_UNSAFE(FROM, TO, VALUE)                                  \
    [] {                                                              \
        using from_type = FROM;                                       \
        using to_type = TO;                                           \
        const auto value = static_cast<from_type>(VALUE);             \
        REQUIRE_THROWS((void)num_collect::safe_cast<to_type>(value)); \
    }()

}  // namespace

TEST_CASE("num_collect::safe_cast (integers)") {
    SECTION("unsigned to unsigned") {
        TEST_SAFE(std::uint32_t, std::uint16_t, 0);
        TEST_SAFE(std::uint32_t, std::uint16_t, 0xFFFF);
        TEST_UNSAFE(std::uint32_t, std::uint16_t, 0x10000);

        TEST_SAFE(std::uint32_t, std::uint32_t, 0);
        TEST_SAFE(std::uint32_t, std::uint32_t, 0xFFFFFFFF);
    }

    SECTION("unsigned to signed") {
        TEST_SAFE(std::uint32_t, std::int32_t, 0);
        TEST_SAFE(std::uint32_t, std::int32_t, 0x7FFFFFFF);
        TEST_UNSAFE(std::uint32_t, std::int32_t, 0x80000000);

        TEST_SAFE(std::uint32_t, std::int32_t, 0);
        TEST_SAFE(std::uint32_t, std::int64_t, 0xFFFFFFFF);
    }

    SECTION("signed to unsigned") {
        TEST_UNSAFE(std::int32_t, std::uint16_t, 0x80000000);
        TEST_UNSAFE(std::int32_t, std::uint16_t, -1);
        TEST_SAFE(std::int32_t, std::uint16_t, 0);
        TEST_SAFE(std::int32_t, std::uint16_t, 0xFFFF);
        TEST_UNSAFE(std::int32_t, std::uint16_t, 0x10000);

        TEST_UNSAFE(std::int32_t, std::uint32_t, 0x80000000);
        TEST_UNSAFE(std::int32_t, std::uint32_t, -1);
        TEST_SAFE(std::int32_t, std::uint32_t, 0);
        TEST_SAFE(std::int32_t, std::uint32_t, 0x7FFFFFFF);
    }

    SECTION("signed to signed") {
        TEST_UNSAFE(std::int32_t, std::int16_t, -32769);
        TEST_SAFE(std::int32_t, std::int16_t, -32768);
        TEST_SAFE(std::int32_t, std::int16_t, 0);
        TEST_SAFE(std::int32_t, std::int16_t, 32767);
        TEST_UNSAFE(std::int32_t, std::int16_t, 32768);

        TEST_SAFE(std::int32_t, std::int32_t, 0x80000000);
        TEST_SAFE(std::int32_t, std::int32_t, 0);
        TEST_SAFE(std::int32_t, std::int32_t, 0x7FFFFFFF);
    }
}
