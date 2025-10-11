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
 * \brief Test of mathematical functions of quad class.
 */
#include "num_collect/multi_double/quad_math.h"

#include <limits>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::multi_double::abs") {
    using num_collect::multi_double::abs;
    using num_collect::multi_double::quad;

    SECTION("calculate") {
        CHECK(abs(quad(std::numeric_limits<double>::min())) ==
            quad(std::numeric_limits<double>::min()));
        CHECK(abs(quad(0.0, std::numeric_limits<double>::min())) ==
            quad(0.0, std::numeric_limits<double>::min()));
        CHECK(abs(quad(0.0, 0.0)) == quad(0.0, 0.0));
        CHECK(abs(quad(0.0, -std::numeric_limits<double>::min())) ==
            quad(0.0, std::numeric_limits<double>::min()));
        CHECK(abs(quad(-std::numeric_limits<double>::min())) ==
            quad(std::numeric_limits<double>::min()));
    }
}
