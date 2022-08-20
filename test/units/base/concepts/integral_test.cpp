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
 * \brief Test of integral class.
 */
#include "num_collect/base/concepts/integral.h"

#include <cstdint>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::concepts::integral") {
    using num_collect::concepts::integral;

    SECTION("check") {
        STATIC_REQUIRE(integral<int>);
        STATIC_REQUIRE(integral<std::int64_t>);
        STATIC_REQUIRE(integral<std::uint16_t>);
        STATIC_REQUIRE_FALSE(integral<float>);
        STATIC_REQUIRE_FALSE(integral<double>);
        STATIC_REQUIRE_FALSE(integral<Eigen::MatrixXd>);
    }
}
