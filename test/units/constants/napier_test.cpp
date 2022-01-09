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
 * \brief Test of napier
 */
#include "num_collect/constants/napier.h"

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::constants::napier") {
    SECTION("double") {
        constexpr double val = num_collect::constants::napier<double>;
        const double reference = std::exp(1.0);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("float") {
        constexpr float val = num_collect::constants::napier<float>;
        const float reference = std::exp(1.0F);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}
