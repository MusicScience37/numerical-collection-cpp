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
 * \brief Test of getter_of class.
 */
#include "num_collect/logging/concepts/getter_of.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::concepts::getter_of") {
    using num_collect::logging::concepts::getter_of;

    SECTION("check") {
        STATIC_REQUIRE(getter_of<int(), int>);
        STATIC_REQUIRE(getter_of<const std::string&(), std::string>);
        STATIC_REQUIRE_FALSE(getter_of<double(), int>);
    }
}
