/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of kahan_adder concept.
 */
#include "num_collect/util/concepts/kahan_addable.h"

#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::concepts::kahan_addable") {
    using num_collect::util::concepts::kahan_addable;

    SECTION("check") {
        STATIC_REQUIRE(kahan_addable<float>);
        STATIC_REQUIRE(kahan_addable<double>);
        STATIC_REQUIRE(kahan_addable<Eigen::VectorXd>);
        STATIC_REQUIRE_FALSE(kahan_addable<std::string>);
    }
}
