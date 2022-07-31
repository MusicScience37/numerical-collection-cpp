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
 * \brief Test of invocable_as concept.
 */
#include "num_collect/base/concepts/invocable_as.h"

#include <cstdint>
#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::base::concepts::invocable_as") {
    using num_collect::base::concepts::invocable_as;

    SECTION("check") {
        const auto func1 = [](std::uint32_t x) { return x; };
        STATIC_CHECK(
            invocable_as<decltype(func1), std::uint32_t(std::uint32_t)>);

        const auto func2 = [](std::uint32_t x) { return std::string(); };
        STATIC_CHECK_FALSE(
            invocable_as<decltype(func2), std::uint32_t(std::uint32_t)>);
    }
}
