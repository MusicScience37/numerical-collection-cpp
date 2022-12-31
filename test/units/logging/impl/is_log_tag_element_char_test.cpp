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
 * \brief Test of is_log_tag_element_char function.
 */
#include "num_collect/logging/impl/is_log_tag_element_char.h"

#include <cctype>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::impl::is_log_tag_element_char") {
    using num_collect::logging::impl::is_log_tag_element_char;

    SECTION("valid characters") {
        constexpr std::size_t upper_bound = 256;
        for (std::size_t i = 0; i < upper_bound; ++i) {
            INFO("i = " << i);
            const auto c = static_cast<char>(i);

            const bool expected = (std::isalnum(c) != 0) || (c == '_');
            CHECK(is_log_tag_element_char(c) == expected);
        }
    }
}
