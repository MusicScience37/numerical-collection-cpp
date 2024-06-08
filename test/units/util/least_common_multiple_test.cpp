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
 * \brief Test of least_common_multiple function.
 */
#include "num_collect/util/least_common_multiple.h"

#include <cstdint>
#include <numeric>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

TEST_CASE("num_collect::util::least_common_multiple") {
    using num_collect::util::least_common_multiple;

    SECTION("calculate at runtime") {
        const int a = 234;
        const int b = 12;
        const int expected = std::lcm(a, b);
        const int result = least_common_multiple(a, b);
        CHECK(result == expected);
    }

    SECTION("calculate at compile-time") {
        constexpr int a = 234;
        constexpr int b = 12;
        constexpr int expected = std::lcm(a, b);
        constexpr int result = least_common_multiple(a, b);
        CHECK(result == expected);
    }

    SECTION("calculate random problems") {
        static constexpr std::int32_t min_input = 1;
        static constexpr std::int32_t max_input = 0x7FFF;
        static constexpr std::int32_t num_inputs = 10;

        const std::int32_t a = GENERATE(Catch::Generators::take(
            num_inputs, Catch::Generators::random(min_input, max_input)));
        const std::int32_t b = GENERATE(Catch::Generators::take(
            num_inputs, Catch::Generators::random(min_input, max_input)));

        const std::int32_t expected = std::lcm(a, b);
        const std::int32_t result = least_common_multiple(a, b);
        CHECK(result == expected);
    }

    SECTION("validation of inputs") {
        CHECK(least_common_multiple(1, 1) == 1);
        CHECK_THROWS((void)least_common_multiple(1, 0));
        CHECK_THROWS((void)least_common_multiple(0, 1));
        CHECK_THROWS((void)least_common_multiple(0, 0));
        CHECK_THROWS((void)least_common_multiple(-1, 1));
        CHECK_THROWS((void)least_common_multiple(1, -1));
    }

    SECTION("check of overflow") {
        CHECK_THROWS(
            (void)least_common_multiple(static_cast<std::uint32_t>(0x10000),
                static_cast<std::uint32_t>(0x10001)));
    }
}
