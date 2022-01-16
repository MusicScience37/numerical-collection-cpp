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
 * \brief Test of implicitly_convertible_to concept.
 */
#include "num_collect/base/concepts/implicitly_convertible_to.h"

#include <string>
#include <string_view>
#include <thread>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::base::concepts::implicitly_convertible_to") {
    using num_collect::base::concepts::implicitly_convertible_to;

    SECTION("check") {
        STATIC_REQUIRE(implicitly_convertible_to<std::string, std::string>);
        STATIC_REQUIRE(implicitly_convertible_to<int, int>);
        STATIC_REQUIRE(
            implicitly_convertible_to<std::string, std::string_view>);
        STATIC_REQUIRE_FALSE(
            implicitly_convertible_to<std::thread, std::string>);
    }
}
