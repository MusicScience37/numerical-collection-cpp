/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of static_stack class.
 */
#include "num_collect/util/static_stack.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::static_stack") {
    using num_collect::util::static_stack;

    SECTION("create an empty stack") {
        constexpr std::size_t size = 10;
        static_stack<int, size> stack;

        CHECK(stack.empty());
        CHECK(stack.size() == 0U);  // NOLINT(*-size-empty)
    }

    SECTION("push an integer") {
        constexpr std::size_t size = 10;
        static_stack<int, size> stack;

        constexpr int value1 = 123;
        CHECK_NOTHROW(stack.push(value1));
        CHECK_FALSE(stack.empty());
        CHECK(stack.size() == 1U);
        CHECK(stack.top() == value1);

        SECTION("pop the value") {
            CHECK_NOTHROW(stack.pop());
            CHECK(stack.empty());
            CHECK(stack.size() == 0U);  // NOLINT(*-size-empty)
        }

        SECTION("push another value") {
            constexpr int value2 = 4567;
            CHECK_NOTHROW(stack.push(value2));
            CHECK_FALSE(stack.empty());
            CHECK(stack.size() == 2U);
            CHECK(stack.top() == value2);

            SECTION("pop the last value") {
                CHECK_NOTHROW(stack.pop());
                CHECK_FALSE(stack.empty());
                CHECK(stack.size() == 1U);
                CHECK(stack.top() == value1);

                SECTION("pop the first value") {
                    CHECK_NOTHROW(stack.pop());
                    CHECK(stack.empty());
                    CHECK(stack.size() == 0U);  // NOLINT(*-size-empty)
                }

                SECTION("destroy values in destructor") {
                    // No operation. Values are destroyed automatically.
                }
            }

            SECTION("destroy values in destructor") {
                // No operation. Values are destroyed automatically.
            }
        }

        SECTION("destroy values in destructor") {
            // No operation. Values are destroyed automatically.
        }
    }

    SECTION("push a string") {
        constexpr std::size_t size = 10;
        static_stack<std::string, size> stack;

        const std::string value1 = "abc";
        CHECK_NOTHROW(stack.push(value1));
        CHECK_FALSE(stack.empty());
        CHECK(stack.size() == 1U);
        CHECK(stack.top() == value1);

        SECTION("pop the value") {
            CHECK_NOTHROW(stack.pop());
            CHECK(stack.empty());
            CHECK(stack.size() == 0U);  // NOLINT(*-size-empty)
        }

        SECTION("push another value") {
            const std::string value2 = "de";
            CHECK_NOTHROW(stack.push(value2));
            CHECK_FALSE(stack.empty());
            CHECK(stack.size() == 2U);
            CHECK(stack.top() == value2);

            SECTION("pop the last value") {
                CHECK_NOTHROW(stack.pop());
                CHECK_FALSE(stack.empty());
                CHECK(stack.size() == 1U);
                CHECK(stack.top() == value1);

                SECTION("pop the first value") {
                    CHECK_NOTHROW(stack.pop());
                    CHECK(stack.empty());
                    CHECK(stack.size() == 0U);  // NOLINT(*-size-empty)
                }

                SECTION("destroy values in destructor") {
                    // No operation. Values are destroyed automatically.
                }
            }

            SECTION("destroy values in destructor") {
                // No operation. Values are destroyed automatically.
            }
        }

        SECTION("destroy values in destructor") {
            // No operation. Values are destroyed automatically.
        }
    }

    SECTION("push too many values") {
        constexpr std::size_t size = 5;
        static_stack<int, size> stack;

        CHECK_NOTHROW(stack.push(1));
        CHECK_NOTHROW(stack.push(2));
        CHECK_NOTHROW(stack.push(3));
        CHECK_NOTHROW(stack.push(4));
        CHECK_NOTHROW(stack.push(5));
        CHECK_THROWS(stack.push(6));

        CHECK_FALSE(stack.empty());
        CHECK(stack.size() == 5U);
        CHECK(stack.top() == 5);
    }
}
