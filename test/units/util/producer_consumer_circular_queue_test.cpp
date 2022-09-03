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
 * \brief Test of producer_consumer_circular_queue class.
 */
#include "num_collect/util/producer_consumer_circular_queue.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::producer_consumer_circular_queue<string>") {
    using queue_type =
        num_collect::util::producer_consumer_circular_queue<std::string>;

    SECTION("check of noexcept") {
        constexpr std::size_t size = 10;
        queue_type queue{size};

        STATIC_CHECK_FALSE(noexcept(queue.try_emplace("abc")));

        std::string str;
        STATIC_CHECK(noexcept(queue.try_emplace(std::move(str))));

        STATIC_CHECK(noexcept(queue.try_pop(str)));

        (void)queue;
        (void)str;
    }

    SECTION("push an element") {
        constexpr std::size_t size = 10;
        queue_type queue{size};

        CHECK(queue.try_emplace("abc"));
    }

    SECTION("push and pop an element") {
        constexpr std::size_t size = 10;
        queue_type queue{size};

        CHECK(queue.try_emplace("abc"));

        std::string out;
        CHECK(queue.try_pop(out));
        CHECK(out == "abc");
    }

    SECTION("push multiple elements") {
        constexpr std::size_t size = 10;
        queue_type queue{size};

        CHECK(queue.try_emplace("abc"));
        CHECK(queue.try_emplace("def"));
    }

    SECTION("push and pop multiple elements") {
        constexpr std::size_t size = 10;
        queue_type queue{size};

        CHECK(queue.try_emplace("abc"));
        CHECK(queue.try_emplace("def"));

        std::string out;
        CHECK(queue.try_pop(out));
        CHECK(out == "abc");
        CHECK(queue.try_pop(out));
        CHECK(out == "def");
        CHECK_FALSE(queue.try_pop(out));
    }

    SECTION("loop of buffer") {
        constexpr std::size_t size = 2;
        queue_type queue{size};

        CHECK(queue.try_emplace("abc"));
        CHECK(queue.try_emplace("def"));
        CHECK_FALSE(queue.try_emplace("ghi"));

        std::string out;
        CHECK(queue.try_pop(out));
        CHECK(out == "abc");
        CHECK(queue.try_emplace("ghi"));

        CHECK(queue.try_pop(out));
        CHECK(out == "def");
        CHECK(queue.try_emplace("jkl"));

        CHECK(queue.try_pop(out));
        CHECK(out == "ghi");
        CHECK(queue.try_emplace("mno"));

        CHECK(queue.try_pop(out));
        CHECK(out == "jkl");
        CHECK(queue.try_emplace("pqr"));

        CHECK(queue.try_pop(out));
        CHECK(out == "mno");
        CHECK(queue.try_emplace("stu"));

        CHECK(queue.try_pop(out));
        CHECK(out == "pqr");
        CHECK(queue.try_emplace("vwx"));

        CHECK(queue.try_pop(out));
        CHECK(out == "stu");

        CHECK(queue.try_pop(out));
        CHECK(out == "vwx");

        CHECK_FALSE(queue.try_pop(out));
    }
}
