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
 * \brief Test of assertion_decomposer class.
 */

#include "num_collect/util/impl/assertion_decomposer.h"

#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

TEST_CASE("num_collect::util::impl::assertion_value") {
    using num_collect::util::impl::assertion_value;

    SECTION("evaluate") {
        REQUIRE(assertion_value<bool>(true).evaluate_to_bool());
        REQUIRE_FALSE(assertion_value<bool>(false).evaluate_to_bool());

        REQUIRE(assertion_value<std::optional<int>>(std::optional<int>(0))
                    .evaluate_to_bool());
        REQUIRE_FALSE(assertion_value<std::optional<int>>(std::optional<int>())
                          .evaluate_to_bool());
    }

    SECTION("format") {
        REQUIRE(fmt::format("{}", assertion_value<bool>(true)) == "true");
        REQUIRE(fmt::format("{}", assertion_value<int>(2)) == "2");
    }
}
