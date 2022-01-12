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
 * \brief Test of formattable concept.
 */
#include "num_collect/base/concepts/formattable.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::concepts::formattable") {
    using num_collect::concepts::formattable;

    SECTION("check") {
        STATIC_REQUIRE(formattable<int>);
        STATIC_REQUIRE(formattable<double>);
        STATIC_REQUIRE(formattable<std::string>);
        STATIC_REQUIRE(formattable<num_collect::index_type>);
    }
}
