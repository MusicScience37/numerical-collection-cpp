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
 * \brief Test of get_size function.
 */
#include "num_collect/util/hash_string.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::hash_string") {
    using num_collect::util::hash_string;
    SECTION("calculate") {
        STATIC_REQUIRE(hash_string("abc") != hash_string("ab"));
        STATIC_REQUIRE(hash_string("abc") != hash_string("abd"));
    }
}
