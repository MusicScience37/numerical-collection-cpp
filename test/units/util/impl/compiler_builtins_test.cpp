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
 * \brief Test of macros for built-in functions of compilers.
 */
#include "num_collect/util/impl/compiler_builtins.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_collect/base/index_type.h"

namespace {

[[nodiscard]] constexpr auto call_num_collect_builtin_file() -> const char* {
    return NUM_COLLECT_BUILTIN_FILE();
}

}  // namespace

TEST_CASE("NUM_COLLECT_BUILTIN_FILE") {
    SECTION("get") {
        const std::string result = call_num_collect_builtin_file();
#if NUM_COLLECT_HAS_BUILTIN_FILE
        CHECK_THAT(result,
            Catch::Matchers::ContainsSubstring("compiler_builtins_test.cpp"));
#else
        (void)result;
#endif
    }
}

namespace {

[[nodiscard]] constexpr auto call_num_collect_builtin_function() -> const
    char* {
    return NUM_COLLECT_BUILTIN_FUNCTION();
}

}  // namespace

TEST_CASE("NUM_COLLECT_BUILTIN_FUNCTION") {
    SECTION("get") {
        const std::string result = call_num_collect_builtin_function();
#if NUM_COLLECT_HAS_BUILTIN_FUNCTION
        CHECK(result == "call_num_collect_builtin_function");
#else
        (void)result;
#endif
    }
}

namespace {

[[nodiscard]] constexpr auto call_num_collect_builtin_line()
    -> num_collect::index_type {
    return NUM_COLLECT_BUILTIN_LINE();
}

}  // namespace

TEST_CASE("NUM_COLLECT_BUILTIN_LINE") {
    SECTION("get") {
        const num_collect::index_type result = call_num_collect_builtin_line();
#if NUM_COLLECT_HAS_BUILTIN_LINE
        CHECK(result > 0);
#else
        (void)result;
#endif
    }
}

namespace {

[[nodiscard]] constexpr auto call_num_collect_builtin_column()
    -> num_collect::index_type {
    return NUM_COLLECT_BUILTIN_COLUMN();
}

}  // namespace

TEST_CASE("NUM_COLLECT_BUILTIN_COLUMN") {
    SECTION("get") {
        const num_collect::index_type result =
            call_num_collect_builtin_column();
#if NUM_COLLECT_HAS_BUILTIN_COLUMN
        CHECK(result > 0);
#else
        (void)result;
#endif
    }
}
