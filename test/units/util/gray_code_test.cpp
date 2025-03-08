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
 * \brief Test of functions for gray code.
 */
#include "num_collect/util/gray_code.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::binary_to_gray_code") {
    using num_collect::util::binary_to_gray_code;

    SECTION("convert") {
        CHECK(binary_to_gray_code(0U) == 0U);
        CHECK(binary_to_gray_code(1U) == 0b1U);
        CHECK(binary_to_gray_code(2U) == 0b11U);
        CHECK(binary_to_gray_code(3U) == 0b10U);
        CHECK(binary_to_gray_code(4U) == 0b110U);
        CHECK(binary_to_gray_code(5U) == 0b111U);
        CHECK(binary_to_gray_code(6U) == 0b101U);
        CHECK(binary_to_gray_code(7U) == 0b100U);
        CHECK(binary_to_gray_code(123U) == 70U);
        CHECK(binary_to_gray_code(123456U) == 70496U);
        CHECK(binary_to_gray_code(123456789U) == 83241887U);
        CHECK(binary_to_gray_code(1234567890123U) == 1721359566510U);
    }
}

TEST_CASE("num_collect::util::gray_code_to_binary") {
    using num_collect::util::gray_code_to_binary;

    SECTION("convert") {
        CHECK(gray_code_to_binary(0U) == 0U);
        CHECK(gray_code_to_binary(0b1U) == 1U);
        CHECK(gray_code_to_binary(0b11U) == 2U);
        CHECK(gray_code_to_binary(0b10U) == 3U);
        CHECK(gray_code_to_binary(0b110U) == 4U);
        CHECK(gray_code_to_binary(0b111U) == 5U);
        CHECK(gray_code_to_binary(0b101U) == 6U);
        CHECK(gray_code_to_binary(0b100U) == 7U);
        CHECK(gray_code_to_binary(70U) == 123U);
        CHECK(gray_code_to_binary(70496U) == 123456U);
        CHECK(gray_code_to_binary(83241887U) == 123456789U);
        CHECK(gray_code_to_binary(1721359566510U) == 1234567890123U);
    }
}
