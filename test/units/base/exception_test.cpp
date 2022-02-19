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
 * \brief Test of exceptions.
 */
#include "num_collect/base/exception.h"

#include <type_traits>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("exceptions", "", num_collect::num_collect_exception,
    num_collect::assertion_failure, num_collect::algorithm_failure,
    num_collect::file_error) {
    using test_type = TestType;

    SECTION("basic functions") {
        STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<test_type>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<test_type>);
    }

    SECTION("construct") {
        const auto message = std::string("test message");
        auto e = test_type(message);
        REQUIRE_THAT(e.what(), Catch::Matchers::ContainsSubstring(message));
#if NUM_COLLECT_HAS_SOURCE_LOCATION
        REQUIRE_THAT(
            e.what(), Catch::Matchers::ContainsSubstring("exception_test.cpp"));
#endif
    }
}
