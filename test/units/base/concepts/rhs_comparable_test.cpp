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
 * \brief Test of rhs_comparable concept.
 */
#include "num_collect/base/concepts/rhs_comparable.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::base::concepts::rhs_less_than_comparable") {
    using num_collect::base::concepts::rhs_less_than_comparable;

    SECTION("check") {
        STATIC_REQUIRE(rhs_less_than_comparable<int, int>);
        STATIC_REQUIRE_FALSE(rhs_less_than_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator<(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(rhs_less_than_comparable<right, left>);
    }
}

TEST_CASE("num_collect::base::concepts::rhs_less_than_or_equal_to_comparable") {
    using num_collect::base::concepts::rhs_less_than_or_equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(rhs_less_than_or_equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(
            rhs_less_than_or_equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator<=(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(rhs_less_than_or_equal_to_comparable<right, left>);
    }
}

TEST_CASE("num_collect::base::concepts::rhs_greater_than_comparable") {
    using num_collect::base::concepts::rhs_greater_than_comparable;

    SECTION("check") {
        STATIC_REQUIRE(rhs_greater_than_comparable<int, int>);
        STATIC_REQUIRE_FALSE(rhs_greater_than_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator>(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(rhs_greater_than_comparable<right, left>);
    }
}

TEST_CASE(
    "num_collect::base::concepts::rhs_greater_than_or_equal_to_comparable") {
    using num_collect::base::concepts::rhs_greater_than_or_equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(rhs_greater_than_or_equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(
            rhs_greater_than_or_equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator>=(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(rhs_greater_than_or_equal_to_comparable<right, left>);
    }
}

TEST_CASE("num_collect::base::concepts::rhs_equal_to_comparable") {
    using num_collect::base::concepts::rhs_equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(rhs_equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(rhs_equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator==(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(rhs_equal_to_comparable<right, left>);
    }
}

TEST_CASE("num_collect::base::concepts::rhs_not_equal_to_comparable") {
    using num_collect::base::concepts::rhs_not_equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(rhs_not_equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(rhs_not_equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator!=(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(rhs_not_equal_to_comparable<right, left>);
    }
}

TEST_CASE("num_collect::base::concepts::rhs_comparable") {
    using num_collect::base::concepts::rhs_comparable;

    SECTION("check") {
        STATIC_REQUIRE(rhs_comparable<int, int>);
        STATIC_REQUIRE_FALSE(rhs_comparable<int, std::string>);
    }
}
