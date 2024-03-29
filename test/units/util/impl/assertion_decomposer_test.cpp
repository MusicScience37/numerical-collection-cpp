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
 * \brief Test of assertion_decomposer class.
 */
#include "num_collect/util/impl/assertion_decomposer.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "num_collect/util/comparators.h"

TEST_CASE("num_collect::util::impl::assertion_value") {
    using num_collect::util::impl::assertion_value;

    SECTION("evaluate") {
        CHECK(assertion_value<bool>(true).evaluate_to_bool());
        CHECK_FALSE(assertion_value<bool>(false).evaluate_to_bool());

        CHECK(assertion_value<std::optional<int>>(std::optional<int>(0))
                  .evaluate_to_bool());
        CHECK_FALSE(assertion_value<std::optional<int>>(std::optional<int>())
                        .evaluate_to_bool());
    }

    SECTION("format") {
        CHECK(fmt::format("{}", assertion_value<bool>(true)) == "true");
        CHECK(fmt::format("{}", assertion_value<int>(2)) == "2");
    }
}

TEST_CASE("num_collect::util::impl::assertion_comparison") {
    using num_collect::util::impl::assertion_comparison;
    using num_collect::util::impl::assertion_value;

    SECTION("evaluate") {
        SECTION("to true") {
            const int left_val = 3;
            const int right_val = 5;
            const auto left = assertion_value<int>(left_val);
            const auto right = assertion_value<int>(right_val);
            constexpr std::string_view operator_str = "<";

            const auto comp = assertion_comparison<int, int, std::less<>>(
                left, right, operator_str);
            CHECK(comp.evaluate_to_bool());
        }

        SECTION("to false") {
            const int left_val = 7;
            const int right_val = 5;
            const auto left = assertion_value<int>(left_val);
            const auto right = assertion_value<int>(right_val);
            constexpr std::string_view operator_str = "<";

            const auto comp = assertion_comparison<int, int, std::less<>>(
                left, right, operator_str);
            CHECK_FALSE(comp.evaluate_to_bool());
        }
    }

    SECTION("format") {
        const int left_val = 3;
        const int right_val = 5;
        const auto left = assertion_value<int>(left_val);
        const auto right = assertion_value<int>(right_val);
        constexpr std::string_view operator_str = "<";

        const auto comp = assertion_comparison<int, int, std::less<>>(
            left, right, operator_str);

        CHECK(fmt::format("{}", comp) == "3 < 5");
    }

    SECTION("create using operators") {
        const int left_val = 3;
        const int right_val = 5;
        const auto left = assertion_value<int>(left_val);

        SECTION("operator<") {
            const auto comp = left < right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison<int, int,
                    num_collect::util::less<int>>>);
            CHECK(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 < 5");
        }

        SECTION("operator<=") {
            const auto comp = left <= right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison<int, int,
                    num_collect::util::less_equal<int>>>);
            CHECK(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 <= 5");
        }

        SECTION("operator>") {
            const auto comp = left > right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison<int, int,
                    num_collect::util::greater<int>>>);
            CHECK_FALSE(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 > 5");
        }

        SECTION("operator>=") {
            const auto comp = left >= right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison<int, int,
                    num_collect::util::greater_equal<int>>>);
            CHECK_FALSE(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 >= 5");
        }

        SECTION("operator==") {
            const auto comp = left == right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison<int, int,
                    num_collect::util::equal<int>>>);
            CHECK_FALSE(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 == 5");
        }

        SECTION("operator!=") {
            const auto comp = left != right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison<int, int,
                    num_collect::util::not_equal<int>>>);
            CHECK(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 != 5");
        }
    }
}

TEST_CASE("num_collect::util::impl::assertion_comparison2") {
    using num_collect::util::impl::assertion_comparison;
    using num_collect::util::impl::assertion_comparison2;
    using num_collect::util::impl::assertion_value;

    SECTION("evaluate") {
        SECTION("to true") {
            const int left_val = 3;
            const int middle_val = 4;
            const int right_val = 5;
            const auto left = assertion_value<int>(left_val);
            const auto middle = assertion_value<int>(middle_val);
            const auto right = assertion_value<int>(right_val);
            constexpr std::string_view operator_str = "<";

            const auto comp =
                assertion_comparison2<int, int, int, std::less<>, std::less<>>(
                    assertion_comparison<int, int, std::less<>>(
                        left, middle, operator_str),
                    right, operator_str);
            CHECK(comp.evaluate_to_bool());
        }

        SECTION("to false (left)") {
            const int left_val = 3;
            const int middle_val = 3;
            const int right_val = 5;
            const auto left = assertion_value<int>(left_val);
            const auto middle = assertion_value<int>(middle_val);
            const auto right = assertion_value<int>(right_val);
            constexpr std::string_view operator_str = "<";

            const auto comp =
                assertion_comparison2<int, int, int, std::less<>, std::less<>>(
                    assertion_comparison<int, int, std::less<>>(
                        left, middle, operator_str),
                    right, operator_str);
            CHECK_FALSE(comp.evaluate_to_bool());
        }

        SECTION("to false (right)") {
            const int left_val = 3;
            const int middle_val = 5;
            const int right_val = 5;
            const auto left = assertion_value<int>(left_val);
            const auto middle = assertion_value<int>(middle_val);
            const auto right = assertion_value<int>(right_val);
            constexpr std::string_view operator_str = "<";

            const auto comp =
                assertion_comparison2<int, int, int, std::less<>, std::less<>>(
                    assertion_comparison<int, int, std::less<>>(
                        left, middle, operator_str),
                    right, operator_str);
            CHECK_FALSE(comp.evaluate_to_bool());
        }
    }

    SECTION("format") {
        const int left_val = 3;
        const int middle_val = 4;
        const int right_val = 5;
        const auto left = assertion_value<int>(left_val);
        const auto middle = assertion_value<int>(middle_val);
        const auto right = assertion_value<int>(right_val);
        constexpr std::string_view operator_str = "<";

        const auto comp =
            assertion_comparison2<int, int, int, std::less<>, std::less<>>(
                assertion_comparison<int, int, std::less<>>(
                    left, middle, operator_str),
                right, operator_str);

        CHECK(fmt::format("{}", comp) == "3 < 4 < 5");
    }

    SECTION("create using operators") {
        const int left_val = 3;
        const int middle_val = 4;
        const int right_val = 5;
        const auto left = assertion_value<int>(left_val);
        const auto middle = assertion_value<int>(middle_val);
        const auto comp_left =
            assertion_comparison<int, int, std::less<>>(left, middle, "<");

        SECTION("operator<") {
            const auto comp = comp_left < right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison2<int, int, int, std::less<>,
                    num_collect::util::less<int>>>);
            CHECK(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 < 4 < 5");
        }

        SECTION("operator<=") {
            const auto comp = comp_left <= right_val;

            STATIC_CHECK(std::is_same_v<decltype(comp),
                const assertion_comparison2<int, int, int, std::less<>,
                    num_collect::util::less_equal<int>>>);
            CHECK(comp.evaluate_to_bool());
            CHECK(fmt::format("{}", comp) == "3 < 4 <= 5");
        }
    }
}

TEST_CASE("num_collect::util::impl::assertion_decomposer") {
    using num_collect::util::impl::assertion_decomposer;

    SECTION("simple booleans") {
        CHECK((assertion_decomposer() < true).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < false).evaluate_to_bool());

        CHECK_FALSE((assertion_decomposer() < std::shared_ptr<int>())
                        .evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < std::shared_ptr<int>())
                        .evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < true) == "true");
    }

    SECTION("smart pointers") {
        CHECK_FALSE((assertion_decomposer() < std::shared_ptr<int>())
                        .evaluate_to_bool());
        CHECK((assertion_decomposer() < std::make_shared<int>(0))
                  .evaluate_to_bool());

        CHECK(fmt::format(
                  "{}", assertion_decomposer() < std::shared_ptr<int>()) ==
            "<unformattable-value>");
        CHECK(fmt::format(
                  "{}", assertion_decomposer() < std::make_shared<int>(0)) ==
            "<unformattable-value>");
    }

    SECTION("comparison with single operator<") {
        CHECK((assertion_decomposer() < 1 < 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 < 1).evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < 1 < 2) == "1 < 2");
    }

    SECTION("comparison with single operator<=") {
        CHECK((assertion_decomposer() < 1 <= 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 <= 0).evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < 1 <= 2) == "1 <= 2");
    }

    SECTION("comparison with single operator>") {
        CHECK((assertion_decomposer() < 3 > 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 > 1).evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < 3 > 2) == "3 > 2");
    }

    SECTION("comparison with single operator>=") {
        CHECK((assertion_decomposer() < 3 >= 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 >= 2).evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < 3 >= 2) == "3 >= 2");
    }

    SECTION("comparison with single operator==") {
        CHECK((assertion_decomposer() < 2 == 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 == 2).evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < 2 == 2) == "2 == 2");
    }

    SECTION("comparison with single operator!=") {
        CHECK((assertion_decomposer() < 3 != 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 2 != 2).evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < 2 != 2) == "2 != 2");
    }

    SECTION("comparison with multiple operators") {
        CHECK((assertion_decomposer() < 1 < 2 < 3).evaluate_to_bool());
        CHECK((assertion_decomposer() < 1 < 2 <= 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 < 1 < 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 < 2 < 2).evaluate_to_bool());
        CHECK_FALSE((assertion_decomposer() < 1 < 2 <= 1).evaluate_to_bool());

        CHECK(fmt::format("{}", assertion_decomposer() < 1 < 2 < 3) ==
            "1 < 2 < 3");
        CHECK(fmt::format("{}", assertion_decomposer() < 1 < 2 <= 3) ==
            "1 < 2 <= 3");
    }
}
