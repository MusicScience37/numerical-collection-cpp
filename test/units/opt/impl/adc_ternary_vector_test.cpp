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
 * \brief Test of adc_ternary_vector class.
 */
#include "num_collect/opt/impl/adc_ternary_vector.h"

#include <type_traits>

#include <Eigen/Core>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::opt::impl::adc_ternary_vector") {
    using num_collect::opt::impl::adc_ternary_vector;

    SECTION("basic functions") {
        constexpr num_collect::index_type dim = 4;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type = adc_ternary_vector<variable_type, dim>;

        STATIC_REQUIRE(std::is_copy_constructible_v<ternary_vector_type>);
        STATIC_REQUIRE(std::is_copy_assignable_v<ternary_vector_type>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<ternary_vector_type>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<ternary_vector_type>);
    }

    SECTION("default constructor") {
        constexpr num_collect::index_type dim = 4;
        constexpr num_collect::index_type max_digits = 3;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;

        ternary_vector_type vec;
        REQUIRE(vec.dim() == dim);
    }

    SECTION("construct with number of dimensions") {
        constexpr num_collect::index_type dim = 4;
        constexpr num_collect::index_type max_digits = 3;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;

        const auto vec = ternary_vector_type(dim);
        REQUIRE(vec.dim() == dim);
        for (num_collect::index_type i = 0; i < dim; ++i) {
            INFO("i = " << i);
            REQUIRE(vec.digits(i) == 0);
        }
    }

    SECTION("add digits") {
        constexpr num_collect::index_type dim = 3;
        constexpr num_collect::index_type max_digits = 3;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;

        auto vec = ternary_vector_type(dim);
        const auto& const_vec = vec;
        REQUIRE(vec.dim() == dim);
        CHECK(const_vec.current_max_digits() == 0);
        CHECK(const_vec.next_divided_dimension_index() == 0);

        auto [dimension_index, digit_index] = vec.push_back(1);
        CHECK(dimension_index == 0);
        CHECK(digit_index == 0);
        CHECK(const_vec.digits(0) == 1);
        CHECK(const_vec.digits(1) == 0);
        CHECK(const_vec.digits(2) == 0);
        CHECK(const_vec(0, 0) == 1);
        CHECK(const_vec.current_max_digits() == 1);
        CHECK(const_vec.next_divided_dimension_index() == 1);

        std::tie(dimension_index, digit_index) = vec.push_back(0);
        CHECK(dimension_index == 1);
        CHECK(digit_index == 0);
        CHECK(const_vec.digits(0) == 1);
        CHECK(const_vec.digits(1) == 1);
        CHECK(const_vec.digits(2) == 0);
        CHECK(const_vec(0, 0) == 1);
        CHECK(const_vec(1, 0) == 0);
        CHECK(const_vec.current_max_digits() == 1);
        CHECK(const_vec.next_divided_dimension_index() == 2);

        std::tie(dimension_index, digit_index) = vec.push_back(2);
        CHECK(dimension_index == 2);
        CHECK(digit_index == 0);
        CHECK(const_vec.digits(0) == 1);
        CHECK(const_vec.digits(1) == 1);
        CHECK(const_vec.digits(2) == 1);
        CHECK(const_vec(0, 0) == 1);
        CHECK(const_vec(1, 0) == 0);
        CHECK(const_vec(2, 0) == 2);
        CHECK(const_vec.current_max_digits() == 1);
        CHECK(const_vec.next_divided_dimension_index() == 0);

        std::tie(dimension_index, digit_index) = vec.push_back(0);
        CHECK(dimension_index == 0);
        CHECK(digit_index == 1);
        CHECK(const_vec.digits(0) == 2);
        CHECK(const_vec.digits(1) == 1);
        CHECK(const_vec.digits(2) == 1);
        CHECK(const_vec(0, 0) == 1);
        CHECK(const_vec(1, 0) == 0);
        CHECK(const_vec(2, 0) == 2);
        CHECK(const_vec(0, 1) == 0);
        CHECK(const_vec.current_max_digits() == 2);
        CHECK(const_vec.next_divided_dimension_index() == 1);
    }

    SECTION("add more digits") {
        constexpr num_collect::index_type dim = 3;
        constexpr num_collect::index_type max_digits = 2;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;
        auto vec = ternary_vector_type(dim);
        REQUIRE(vec.dim() == dim);

        CHECK_NOTHROW(vec.push_back(0));
        CHECK_FALSE(vec.is_full());

        CHECK_NOTHROW(vec.push_back(0));
        CHECK_FALSE(vec.is_full());

        CHECK_NOTHROW(vec.push_back(0));
        CHECK_FALSE(vec.is_full());

        CHECK_NOTHROW(vec.push_back(0));
        CHECK_FALSE(vec.is_full());

        CHECK_NOTHROW(vec.push_back(0));
        CHECK_FALSE(vec.is_full());

        CHECK_NOTHROW(vec.push_back(0));
        CHECK(vec.is_full());

        CHECK_THROWS(vec.push_back(0));
        CHECK(vec.is_full());
    }

    SECTION("compare same vectors") {
        constexpr num_collect::index_type dim = 3;
        constexpr num_collect::index_type max_digits = 2;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;

        auto vec1 = ternary_vector_type(dim);
        vec1.push_back(1);
        auto vec2 = ternary_vector_type(dim);
        vec2.push_back(1);
        CHECK(vec1 == vec2);
    }

    SECTION("compare vectors with different digit") {
        constexpr num_collect::index_type dim = 3;
        constexpr num_collect::index_type max_digits = 2;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;

        auto vec1 = ternary_vector_type(dim);
        vec1.push_back(1);
        auto vec2 = ternary_vector_type(dim);
        vec2.push_back(0);
        CHECK(vec1 != vec2);
    }

    SECTION("get element as double") {
        constexpr num_collect::index_type dim = 1;
        constexpr num_collect::index_type max_digits = 10;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;

        auto vec = ternary_vector_type(dim);
        vec.push_back(0);
        vec.push_back(2);
        vec.push_back(0);
        vec.push_back(1);
        constexpr double expected = 2.0 / 3.0 + 1.0 / 27.0;
        CHECK_THAT(
            vec.elem_as<double>(0), Catch::Matchers::WithinRel(expected));
    }

    SECTION("get as a variable") {
        constexpr num_collect::index_type dim = 2;
        constexpr num_collect::index_type max_digits = 3;
        using variable_type = Eigen::Vector<double, dim>;
        using ternary_vector_type =
            adc_ternary_vector<variable_type, max_digits>;

        auto vec = ternary_vector_type(dim);
        vec.push_back(0);
        vec.push_back(0);
        vec.push_back(2);
        vec.push_back(0);
        vec.push_back(0);
        vec.push_back(1);

        const variable_type lower_bound = variable_type::Constant(-1.0);
        const variable_type width = variable_type::Constant(3.0);
        const variable_type variable = vec.as_variable(lower_bound, width);
        CHECK_THAT(variable(0), Catch::Matchers::WithinRel(1.0));
        CHECK_THAT(variable(1),
            Catch::Matchers::WithinRel(-2.0 / 3.0));  // NOLINT(*-magic-numbers)
    }
}

TEST_CASE("std::hash<num_collect::opt::impl::adc_ternary_vector>") {
    using num_collect::opt::impl::adc_ternary_vector;

    constexpr num_collect::index_type dim = 3;
    constexpr num_collect::index_type max_digits = 5;
    using variable_type = Eigen::Vector<double, dim>;
    using ternary_vector_type = adc_ternary_vector<variable_type, max_digits>;
    const std::hash<ternary_vector_type> hash;

    SECTION("compare same vectors") {
        auto vec1 = ternary_vector_type(dim);
        vec1.push_back(1);
        auto vec2 = ternary_vector_type(dim);
        vec2.push_back(1);
        CHECK(hash(vec1) == hash(vec2));
    }

    SECTION("compare vectors with a different digit") {
        auto vec1 = ternary_vector_type(dim);
        vec1.push_back(1);
        auto vec2 = ternary_vector_type(dim);
        vec2.push_back(2);
        CHECK(hash(vec1) != hash(vec2));
    }

    SECTION("compare vectors with a digit in different dimensions") {
        auto vec1 = ternary_vector_type(dim);
        vec1.push_back(1);
        vec1.push_back(0);
        auto vec2 = ternary_vector_type(dim);
        vec2.push_back(0);
        vec2.push_back(1);
        CHECK(hash(vec1) != hash(vec2));
    }

    SECTION("compare vectors with different digits but same") {
        auto vec1 = ternary_vector_type(dim);
        vec1.push_back(1);
        vec1.push_back(0);
        auto vec2 = ternary_vector_type(dim);
        vec2.push_back(1);
        CHECK(hash(vec1) == hash(vec2));
    }
}
