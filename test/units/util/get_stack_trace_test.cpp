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
 * \brief Test of get_stack_trace function.
 */
#if __has_include(<backward.hpp>)

#include "num_collect/util/get_stack_trace.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("num_collect::get_stack_trace") {
    SECTION("get stack trace") {
        std::string result;
        REQUIRE_NOTHROW(result = num_collect::get_stack_trace());
#ifndef NDEBUG
        REQUIRE_THAT(
            result, Catch::Matchers::Contains("get_stack_trace_test.cpp"));
#endif
    }
}

#endif
