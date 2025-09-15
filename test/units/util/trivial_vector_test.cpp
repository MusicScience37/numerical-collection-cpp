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
 * \brief Test of trivial_vector class.
 */
#include "num_collect/util/trivial_vector.h"

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::util::trivial_vector_compatible") {
    using num_collect::util::trivial_vector_compatible;

    SECTION("check types") {
        // Standard scalar types.
        STATIC_REQUIRE(trivial_vector_compatible<int>);
        STATIC_REQUIRE(trivial_vector_compatible<double>);

        // Types I want to use.
        STATIC_REQUIRE(trivial_vector_compatible<Eigen::Triplet<double>>);

        // Types which are not trivially copyable.
        STATIC_REQUIRE_FALSE(trivial_vector_compatible<std::vector<int>>);

        // Types with large alignment.
        struct alignas(alignof(std::max_align_t) * 2) over_aligned_struct {
            std::array<char, alignof(std::max_align_t) * 2> value;
        };
        STATIC_REQUIRE_FALSE(trivial_vector_compatible<over_aligned_struct>);
    }
}

TEST_CASE("num_collect::util::trivial_vector") {
    using num_collect::index_type;
    using num_collect::util::trivial_vector;

    SECTION("default constructor") {
        trivial_vector<int> vector;

        CHECK(vector.size() == 0);  // NOLINT
        CHECK(vector.empty());
        CHECK(vector.capacity() > 0);
        CHECK(vector.data() != nullptr);
    }

    SECTION("constructor with size") {
        trivial_vector<int> vector(3);

        CHECK(vector.size() == 3);
        CHECK_FALSE(vector.empty());
        CHECK(vector.capacity() >= 3);
        CHECK(vector.data() != nullptr);
        CHECK(vector.at(0) == 0);
        CHECK(vector.at(1) == 0);
        CHECK(vector.at(2) == 0);
    }

    SECTION("constructor with large size") {
        constexpr index_type large_size = 10000;
        trivial_vector<int> vector(large_size);

        CHECK(vector.size() == large_size);
        CHECK_FALSE(vector.empty());
        CHECK(vector.capacity() >= large_size);
        CHECK(vector.data() != nullptr);
    }

    SECTION("constructor with invalid sizes") {
        CHECK_THROWS((void)trivial_vector<int>(-1));  // NOLINT
        CHECK_THROWS(
            (void)trivial_vector<int>(trivial_vector<int>::max_size() + 1));
    }

    SECTION("constructor with size and value") {
        // Type should be deduced from the second argument.
        const auto vector = trivial_vector(3, 2);

        CHECK(vector.size() == 3);
        CHECK_FALSE(vector.empty());
        CHECK(vector.capacity() >= 3);
        CHECK(vector.data() != nullptr);
        CHECK(vector.at(0) == 2);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 2);
    }

    SECTION("constructor with iterators") {
        std::array<int, 3> array = {1, 2, 3};
        trivial_vector<int> vector(array.begin(), array.end());

        CHECK(vector.size() == 3);
        CHECK_FALSE(vector.empty());
        CHECK(vector.capacity() >= 3);
        CHECK(vector.data() != nullptr);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("constructor with pointer and size") {
        std::array<int, 3> array = {1, 2, 3};
        trivial_vector<int> vector(array.data(), 3);

        CHECK(vector.size() == 3);
        CHECK_FALSE(vector.empty());
        CHECK(vector.capacity() >= 3);
        CHECK(vector.data() != nullptr);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("constructor with initializer list") {
        // Type should be deduced from the initializer list.
        const auto vector = trivial_vector{1, 2, 3};

        CHECK(vector.size() == 3);
        CHECK_FALSE(vector.empty());
        CHECK(vector.capacity() >= 3);
        CHECK(vector.data() != nullptr);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("copy constructor") {
        trivial_vector<int> origin{1, 2, 3};

        trivial_vector<int> vector(origin);  // NOLINT

        CHECK(vector.size() == 3);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
        CHECK(origin.size() == 3);
        CHECK(origin.at(0) == 1);
        CHECK(origin.at(1) == 2);
        CHECK(origin.at(2) == 3);
    }

    SECTION("move constructor") {
        trivial_vector<int> origin{1, 2, 3};

        trivial_vector<int> vector(std::move(origin));  // NOLINT

        CHECK(vector.size() == 3);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("copy assignment") {
        trivial_vector<int> origin{1, 2, 3};

        trivial_vector<int> vector;
        vector = origin;

        CHECK(vector.size() == 3);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
        CHECK(origin.size() == 3);
        CHECK(origin.at(0) == 1);
        CHECK(origin.at(1) == 2);
        CHECK(origin.at(2) == 3);
    }

    SECTION("move assignment") {
        trivial_vector<int> origin{1, 2, 3};

        trivial_vector<int> vector;
        vector = std::move(origin);

        CHECK(vector.size() == 3);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("assign an initializer list") {
        trivial_vector<int> vector;
        vector = {1, 2, 3};

        CHECK(vector.size() == 3);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("access elements via at() for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);

        vector.at(0) = 4;  // NOLINT
        vector.at(1) = 5;  // NOLINT
        vector.at(2) = 6;  // NOLINT

        CHECK(vector.at(0) == 4);
        CHECK(vector.at(1) == 5);
        CHECK(vector.at(2) == 6);

        CHECK_THROWS(vector.at(-1));  // NOLINT
        CHECK_THROWS(vector.at(3));   // NOLINT
    }

    SECTION("access elements via at() for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(vector.at(0)), const int&>);

        CHECK_THROWS(vector.at(-1));  // NOLINT
        CHECK_THROWS(vector.at(3));   // NOLINT
    }

    SECTION("access elements via operator[] for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        CHECK(vector[0] == 1);
        CHECK(vector[1] == 2);
        CHECK(vector[2] == 3);

        vector[0] = 4;  // NOLINT
        vector[1] = 5;  // NOLINT
        vector[2] = 6;  // NOLINT

        CHECK(vector[0] == 4);
        CHECK(vector[1] == 5);
        CHECK(vector[2] == 6);
    }

    SECTION("access elements via operator[] for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        CHECK(vector[0] == 1);
        CHECK(vector[1] == 2);
        CHECK(vector[2] == 3);

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(vector[0]), const int&>);
    }

    SECTION("access the first element for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        CHECK(vector.front() == 1);

        vector.front() = 4;  // NOLINT

        CHECK(vector.front() == 4);
    }

    SECTION("access the first element for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        CHECK(vector.front() == 1);

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(vector.front()), const int&>);
    }

    SECTION("access the last element for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        CHECK(vector.back() == 3);

        vector.back() = 4;  // NOLINT

        CHECK(vector.back() == 4);
    }

    SECTION("access the last element for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        CHECK(vector.back() == 3);

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(vector.back()), const int&>);
    }

    SECTION("access data for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        int* data = vector.data();
        REQUIRE(data != nullptr);

        CHECK(data[0] == 1);
        CHECK(data[1] == 2);
        CHECK(data[2] == 3);

        data[0] = 4;  // NOLINT
        data[1] = 5;  // NOLINT
        data[2] = 6;  // NOLINT

        CHECK(vector.at(0) == 4);
        CHECK(vector.at(1) == 5);
        CHECK(vector.at(2) == 6);
    }

    SECTION("access data for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        const int* data = vector.data();
        REQUIRE(data != nullptr);

        CHECK(data[0] == 1);
        CHECK(data[1] == 2);
        CHECK(data[2] == 3);

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(vector.data()[0]), const int&>);
    }

    SECTION("get iterators for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.begin();
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(*iterator == 2);
        iterator++;
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(iterator == vector.end());

        iterator = vector.begin();
        *iterator = 4;  // NOLINT
        CHECK(vector.at(0) == 4);
    }

    SECTION("get iterators for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.begin();
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(*iterator == 2);
        iterator++;
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(iterator == vector.end());

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(*vector.begin()), const int&>);
    }

    SECTION("get iterators with cbegin, cend") {
        const trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.cbegin();
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(*iterator == 2);
        iterator++;
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(iterator == vector.cend());

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(*vector.cbegin()), const int&>);
    }

    SECTION("get reverse iterators for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.rbegin();
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(*iterator == 2);
        iterator++;
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(iterator == vector.rend());

        iterator = vector.rbegin();
        *iterator = 4;  // NOLINT
        CHECK(vector.at(2) == 4);
    }

    SECTION("get reverse iterators for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.rbegin();
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(*iterator == 2);
        iterator++;
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(iterator == vector.rend());

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(*vector.rbegin()), const int&>);
    }

    SECTION("get reverse iterators with crbegin, crend") {
        const trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.crbegin();
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(*iterator == 2);
        iterator++;
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(iterator == vector.crend());

        STATIC_REQUIRE_FALSE(
            std::is_assignable_v<decltype(*vector.crbegin()), const int&>);
    }

    SECTION("accept range-based for loops for non-const object") {
        trivial_vector<int> vector{1, 2, 3};

        int value = 1;
        for (auto& element : vector) {  // NOLINT
            CHECK(element == value);
            element = value + 3;  // NOLINT
            ++value;
        }

        CHECK(vector.at(0) == 4);
        CHECK(vector.at(1) == 5);
        CHECK(vector.at(2) == 6);
    }

    SECTION("accept range-based for loops for const object") {
        const trivial_vector<int> vector{1, 2, 3};

        int value = 1;
        for (const auto& element : vector) {  // NOLINT
            CHECK(element == value);
            ++value;
        }
    }

    SECTION("check whether empty") {
        trivial_vector<int> vector;

        CHECK(vector.empty());

        vector.resize(1);

        CHECK_FALSE(vector.empty());
    }

    SECTION("get size") {
        trivial_vector<int> vector;

        CHECK(vector.size() == 0);  // NOLINT

        vector.resize(3);

        CHECK(vector.size() == 3);
    }

    SECTION("get the maximum size") {
        CHECK(trivial_vector<int>::max_size() > 0);
    }

    SECTION("reserve memory") {
        trivial_vector<int> vector{1, 2, 3};

        constexpr index_type reserved_size = 100;
        vector.reserve(reserved_size);

        CHECK(vector.capacity() >= reserved_size);
        CHECK(vector.size() == 3);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("try to reserve memory with invalid capacities") {
        trivial_vector<int> vector;

        CHECK_THROWS(vector.reserve(-1));  // NOLINT
        CHECK_THROWS(
            vector.reserve(trivial_vector<int>::max_size() + 1));  // NOLINT
    }

    SECTION("get capacity") {
        trivial_vector<int> vector;

        CHECK(vector.capacity() > 0);

        const index_type capacity = vector.capacity();
        vector.reserve(capacity + 1);

        CHECK(vector.capacity() > capacity);
    }

    SECTION("shrink to fit") {
        trivial_vector<int> vector{1, 2, 3};

        vector.shrink_to_fit();

        CHECK(vector.capacity() == 3);
        CHECK(vector.size() == 3);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
    }

    SECTION("shrink to fit for empty vector") {
        trivial_vector<int> vector;

        vector.shrink_to_fit();

        CHECK(vector.capacity() == 1);
        CHECK(vector.size() == 0);  // NOLINT
        CHECK(vector.empty());
    }

    SECTION("clear") {
        trivial_vector<int> vector{1, 2, 3};

        CHECK_FALSE(vector.empty());

        vector.clear();

        CHECK(vector.empty());
        CHECK(vector.size() == 0);  // NOLINT
        CHECK(vector.capacity() >= 3);
    }

    SECTION("insert an element") {
        trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.insert(vector.begin() + 1, 4);  // NOLINT

        CHECK(*iterator == 4);
        CHECK(vector.size() == 4);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 4);
        CHECK(vector.at(2) == 2);
        CHECK(vector.at(3) == 3);
    }

    SECTION("insert an element at the end") {
        trivial_vector<int> vector{1, 2, 3};

        auto iterator = vector.insert(vector.end(), 4);  // NOLINT

        CHECK(*iterator == 4);
        CHECK(vector.size() == 4);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
        CHECK(vector.at(3) == 4);
    }

    SECTION("resize") {
        trivial_vector<int> vector{1, 2, 3};

        vector.resize(5);  // NOLINT(*-magic-numbers)

        CHECK(vector.size() == 5);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
        CHECK(vector.at(3) == 0);
        CHECK(vector.at(4) == 0);

        vector.resize(2);

        CHECK(vector.size() == 2);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
    }

    SECTION("resize with value") {
        trivial_vector<int> vector{1, 2, 3};

        vector.resize(5, 4);  // NOLINT(*-magic-numbers)

        CHECK(vector.size() == 5);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
        CHECK(vector.at(2) == 3);
        CHECK(vector.at(3) == 4);
        CHECK(vector.at(4) == 4);

        vector.resize(2, 5);  // NOLINT(*-magic-numbers)

        CHECK(vector.size() == 2);
        CHECK(vector.at(0) == 1);
        CHECK(vector.at(1) == 2);
    }

    SECTION("resize with large size") {
        trivial_vector<int> vector;

        constexpr index_type large_size = 10000;
        vector.resize(large_size);

        CHECK(vector.size() == large_size);
        CHECK_FALSE(vector.empty());
        CHECK(vector.capacity() >= large_size);
        CHECK(vector.data() != nullptr);
    }

    SECTION("try to resize with invalid sizes") {
        trivial_vector<int> vector;

        CHECK_THROWS(vector.resize(-1));  // NOLINT
        CHECK_THROWS(
            vector.resize(trivial_vector<int>::max_size() + 1));  // NOLINT
    }
}
