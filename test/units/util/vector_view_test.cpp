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
 * \brief Test of vector_view class.
 */
#include "num_collect/util/vector_view.h"

#include <array>
#include <type_traits>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/util/vector.h"

TEST_CASE("num_collect::util::vector_view") {
    using num_collect::util::vector_view;

    SECTION("default constructor") {
        vector_view<double> view;
        CHECK(view.data() == nullptr);
        CHECK(view.size() == 0);  // NOLINT(*-size-empty)
    }

    SECTION("create from a pointer to data and its size") {
        std::array<int, 3> data = {1, 2, 3};

        vector_view<int> view(data.data(), 3);

        STATIC_REQUIRE(std::is_same_v<decltype(view.data()), int*>);
        CHECK(
            static_cast<void*>(view.data()) == static_cast<void*>(data.data()));
        CHECK(view.size() == 3);
        CHECK(view[0] == 1);
        CHECK(view[1] == 2);
        CHECK(view[2] == 3);
    }

    SECTION("create from a constant pointer to data and its size") {
        const std::array<int, 3> data = {1, 2, 3};

        vector_view<const int> view(data.data(), 3);

        STATIC_REQUIRE(std::is_same_v<decltype(view.data()), const int*>);
        CHECK(static_cast<const void*>(view.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(view.size() == 3);
        CHECK(view[0] == 1);
        CHECK(view[1] == 2);
        CHECK(view[2] == 3);
    }

    SECTION("create from a vector") {
        std::vector<int> data{1, 2, 3};

        vector_view<int> view = data;

        STATIC_REQUIRE(std::is_same_v<decltype(view.data()), int*>);
        CHECK(static_cast<const void*>(view.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(view.size() == 3);
        CHECK(view[0] == 1);
        CHECK(view[1] == 2);
        CHECK(view[2] == 3);
    }

    SECTION("create from a Eigen's vector") {
        Eigen::VectorXi data{{1, 2, 3}};

        vector_view<const int> view = data;

        STATIC_REQUIRE(std::is_same_v<decltype(view.data()), const int*>);
        CHECK(static_cast<const void*>(view.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(view.size() == 3);
        CHECK(view[0] == 1);
        CHECK(view[1] == 2);
        CHECK(view[2] == 3);
    }

    SECTION("assign a vector") {
        const std::array<int, 3> data = {1, 2, 3};

        vector_view<const int> view;
        view = data;

        STATIC_REQUIRE(std::is_same_v<decltype(view.data()), const int*>);
        CHECK(static_cast<const void*>(view.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(view.size() == 3);
        CHECK(view[0] == 1);
        CHECK(view[1] == 2);
        CHECK(view[2] == 3);
    }

    SECTION("check conversions") {
        using num_collect::concepts::implicitly_convertible_to;

        STATIC_REQUIRE(
            implicitly_convertible_to<std::vector<int>, vector_view<int>>);
        STATIC_REQUIRE(implicitly_convertible_to<std::vector<int>,
            vector_view<const int>>);
        STATIC_REQUIRE_FALSE(implicitly_convertible_to<const std::vector<int>,
            vector_view<int>>);

        STATIC_REQUIRE(implicitly_convertible_to<num_collect::util::vector<int>,
            vector_view<int>>);

        STATIC_REQUIRE(implicitly_convertible_to<vector_view<int>,
            vector_view<const int>>);
    }

    SECTION("copy in constructor") {
        std::vector<int> data{1, 2, 3};
        vector_view<int> origin = data;

        vector_view<int> copy = origin;

        CHECK(static_cast<const void*>(origin.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(origin.size() == 3);
        CHECK(static_cast<const void*>(copy.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(copy.size() == 3);
    }

    SECTION("move in constructor") {
        std::vector<int> data{1, 2, 3};
        vector_view<int> origin = data;

        // NOLINTNEXTLINE(*-move-*)
        vector_view<int> copy = std::move(origin);

        CHECK(static_cast<const void*>(copy.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(copy.size() == 3);
    }

    SECTION("copy in operator=") {
        std::vector<int> data{1, 2, 3};
        vector_view<int> origin = data;

        vector_view<int> copy;
        copy = origin;

        CHECK(static_cast<const void*>(origin.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(origin.size() == 3);
        CHECK(static_cast<const void*>(copy.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(copy.size() == 3);
    }

    SECTION("move in operator=") {
        std::vector<int> data{1, 2, 3};
        vector_view<int> origin = data;

        vector_view<int> copy;
        // NOLINTNEXTLINE(*-move-*)
        copy = std::move(origin);

        CHECK(static_cast<const void*>(copy.data()) ==
            static_cast<const void*>(data.data()));
        CHECK(copy.size() == 3);
    }

    SECTION("access an element with checks") {
        std::vector<int> data{1, 2};
        vector_view<int> view = data;

        CHECK_THROWS(view.at(-1));
        CHECK(view.at(0) == 1);
        CHECK(view.at(1) == 2);
        CHECK_THROWS(view.at(2));
    }

    SECTION("access to the first and last element") {
        std::vector<int> data{1, 2, 3};
        vector_view<int> view = data;

        CHECK(view.front() == 1);
        CHECK(view.back() == 3);
    }

    SECTION("access data using range-based for (begin() and end())") {
        std::vector<int> data{1, 2, 3};
        vector_view<int> view = data;

        for (int& value : view) {
            ++value;
        }
        CHECK(view[0] == 2);
        CHECK(view[1] == 3);
        CHECK(view[2] == 4);
    }

    SECTION("access data using cbegin() and cend()") {
        std::vector<int> data{1, 2, 3};
        vector_view<int> view = data;

        std::vector<int> actual_data;
        // NOLINTNEXTLINE(*-loop-convert)
        for (auto iter = view.cbegin(); iter != view.cend(); ++iter) {
            STATIC_REQUIRE(std::is_same_v<decltype(*iter), const int&>);
            actual_data.push_back(*iter);
        }
    }

    SECTION("check whether a vector is empty") {
        std::vector<int> data1{};
        std::vector<int> data2{1};

        CHECK(vector_view<int>{data1}.empty());
        CHECK_FALSE(vector_view<int>{data2}.empty());
    }
}
