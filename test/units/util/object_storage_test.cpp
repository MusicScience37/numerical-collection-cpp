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
 * \brief Test of object_storage class.
 */
#include "num_collect/util/object_storage.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::object_storage") {
    using num_collect::util::object_storage;

    SECTION("emplace and reset") {
        object_storage<std::string> storage;
        const object_storage<std::string>& const_storage = storage;
        REQUIRE_NOTHROW(storage.emplace("abc"));
        CHECK(storage.get_ref() == "abc");
        CHECK(const_storage.get_ref() == "abc");
        REQUIRE_NOTHROW(storage.reset());
    }
}
