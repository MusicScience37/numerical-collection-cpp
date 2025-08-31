/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of combination function.
 */
#include "num_collect/util/combination.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::combination") {
    using num_collect::util::combination;

    SECTION("calculate combinations") {
        STATIC_CHECK(combination(0, 0) == 1);
        STATIC_CHECK(combination(1, 0) == 1);
        STATIC_CHECK(combination(1, 1) == 1);
        STATIC_CHECK(combination(3, 1) == 3);
        STATIC_CHECK(combination(4, 2) == 6);
        STATIC_CHECK(combination(7, 5) == 21);
        STATIC_CHECK(combination(11, 7) == 330);
    }

    SECTION("handle special cases") {
        STATIC_CHECK(combination(2, 3) == 0);
        CHECK_THROWS(combination(-1, 0));
        CHECK_THROWS(combination(0, -1));
    }
}
