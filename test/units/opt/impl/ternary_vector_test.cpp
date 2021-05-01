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
 * \brief Test of ternary_vector class.
 */
#include "num_collect/opt/impl/ternary_vector.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::opt::impl::ternary_vector") {
    using num_collect::opt::impl::ternary_vector;

    SECTION("basic functions") {
        using test_type = ternary_vector;
        STATIC_REQUIRE(std::is_copy_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_copy_assignable_v<test_type>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<test_type>);
    }

    SECTION("default constructor") {
        ternary_vector vec;
        REQUIRE(vec.dim() == 0);
    }

    SECTION("construct with number of dimensions") {
        constexpr num_collect::index_type dim = 4;
        const auto vec = ternary_vector(dim);
        REQUIRE(vec.dim() == dim);
        for (num_collect::index_type i = 0; i < dim; ++i) {
            INFO("i = " << i);
            REQUIRE(vec.digits(i) == 0);
        }
    }

    SECTION("change the number of dimensions") {
        ternary_vector vec;
        REQUIRE(vec.dim() == 0);

        constexpr num_collect::index_type dim = 4;
        REQUIRE_NOTHROW(vec.change_dim(dim));
        REQUIRE(vec.dim() == dim);
        for (num_collect::index_type i = 0; i < dim; ++i) {
            INFO("i = " << i);
            REQUIRE(vec.digits(i) == 0);
        }
    }

    SECTION("add digits") {
        constexpr num_collect::index_type dim = 3;
        auto vec = ternary_vector(dim);
        const auto& const_vec = vec;
        REQUIRE(vec.dim() == dim);

        // ignore warnings of magic numbers
        vec.push_back(2, 1);                // NOLINT
        REQUIRE(const_vec.digits(0) == 0);  // NOLINT
        REQUIRE(const_vec.digits(1) == 0);  // NOLINT
        REQUIRE(const_vec.digits(2) == 1);  // NOLINT
        REQUIRE(const_vec(2, 0) == 1);      // NOLINT

        vec.push_back(2, 0);                // NOLINT
        REQUIRE(const_vec.digits(0) == 0);  // NOLINT
        REQUIRE(const_vec.digits(1) == 0);  // NOLINT
        REQUIRE(const_vec.digits(2) == 2);  // NOLINT
        REQUIRE(const_vec(2, 0) == 1);      // NOLINT
        REQUIRE(const_vec(2, 1) == 0);      // NOLINT

        vec.push_back(1, 2);                // NOLINT
        REQUIRE(const_vec.digits(0) == 0);  // NOLINT
        REQUIRE(const_vec.digits(1) == 1);  // NOLINT
        REQUIRE(const_vec.digits(2) == 2);  // NOLINT
        REQUIRE(const_vec(1, 0) == 2);      // NOLINT
        REQUIRE(const_vec(2, 0) == 1);      // NOLINT
        REQUIRE(const_vec(2, 1) == 0);      // NOLINT

        vec.push_back(2, 0);                // NOLINT
        REQUIRE(const_vec.digits(0) == 0);  // NOLINT
        REQUIRE(const_vec.digits(1) == 1);  // NOLINT
        REQUIRE(const_vec.digits(2) == 3);  // NOLINT
        REQUIRE(const_vec(1, 0) == 2);      // NOLINT
        REQUIRE(const_vec(2, 0) == 1);      // NOLINT
        REQUIRE(const_vec(2, 1) == 0);      // NOLINT
        REQUIRE(const_vec(2, 2) == 0);      // NOLINT
    }

    SECTION("add more digits") {
        constexpr num_collect::index_type dim = 3;
        auto vec = ternary_vector(dim);
        REQUIRE(vec.dim() == dim);

        constexpr num_collect::index_type size = 100;
        for (num_collect::index_type i = 0; i < size; ++i) {
            const auto digit = static_cast<ternary_vector::digit_type>(i % 3);
            vec.push_back(0, digit);
        }
        REQUIRE(vec.digits(0) == size);
        for (num_collect::index_type i = 0; i < size; ++i) {
            const auto digit = static_cast<ternary_vector::digit_type>(i % 3);
            INFO("i = " << i);
            CHECK(vec(0, i) == digit);
        }
    }

    SECTION("compare same vectors") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        REQUIRE(vec1 == vec2);
    }

    SECTION("compare vectors with different digit") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 0);  // NOLINT
        REQUIRE(vec1 != vec2);
    }

    SECTION("compare vectors with different digits but same (lhs is larger)") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        vec1.push_back(2, 0);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        REQUIRE(vec1 == vec2);
    }

    SECTION("compare vectors with different digits but same (rhs is larger)") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        vec2.push_back(2, 0);  // NOLINT
        REQUIRE(vec1 == vec2);
    }

    SECTION("compare vectors with non-zero additional digits in lhs") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        REQUIRE(vec1 != vec2);
    }

    SECTION("compare vectors with non-zero additional digits in rhs") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        vec2.push_back(2, 1);  // NOLINT
        REQUIRE(vec1 != vec2);
    }

    SECTION("compare vectors with different dimensions") {
        auto vec1 = ternary_vector(1);
        auto vec2 = ternary_vector(2);
        REQUIRE_THROWS_AS(vec1 == vec2, num_collect::assetion_failure);
    }

    SECTION("get element as double") {
        auto vec = ternary_vector(1);
        vec.push_back(0, 0);  // NOLINT
        vec.push_back(0, 2);  // NOLINT
        vec.push_back(0, 0);  // NOLINT
        vec.push_back(0, 1);  // NOLINT
        constexpr double expected = 2.0 / 3.0 + 1.0 / 27.0;
        REQUIRE_THAT(
            vec.elem_as<double>(0), Catch::Matchers::WithinRel(expected));
    }

    SECTION("get digits of a dimension out of range") {
        constexpr num_collect::index_type dim = 3;
        auto vec = ternary_vector(dim);
        REQUIRE_THROWS_AS(vec.digits(dim), num_collect::assetion_failure);
    }
}

TEST_CASE("std::hash<num_collect::opt::impl::ternary_vector>") {
    using num_collect::opt::impl::ternary_vector;

    const std::hash<ternary_vector> hash;

    SECTION("compare same vectors") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        REQUIRE(hash(vec1) == hash(vec2));
    }

    SECTION("compare vectors with a different digit") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 2);  // NOLINT
        REQUIRE(hash(vec1) != hash(vec2));
    }

    SECTION("compare vectors with a digit in different dimensions") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(1, 1);  // NOLINT
        REQUIRE(hash(vec1) != hash(vec2));
    }

    SECTION("compare vectors with different digits but same (lhs is larger)") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        vec1.push_back(2, 0);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        REQUIRE(hash(vec1) == hash(vec2));
    }

    SECTION("compare vectors with different digits but same (rhs is larger)") {
        constexpr num_collect::index_type dim = 3;
        auto vec1 = ternary_vector(dim);
        vec1.push_back(2, 1);  // NOLINT
        auto vec2 = ternary_vector(dim);
        vec2.push_back(2, 1);  // NOLINT
        vec2.push_back(2, 0);  // NOLINT
        REQUIRE(hash(vec1) == hash(vec2));
    }
}
