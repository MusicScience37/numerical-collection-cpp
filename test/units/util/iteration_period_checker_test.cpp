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
 * \brief Test of iteration_period_checker class.
 */
#include "num_collect/util/iteration_period_checker.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::util::iteration_period_checker") {
    using num_collect::util::iteration_period_checker;

    SECTION("use named functions") {
        constexpr num_collect::index_type period = 3;
        iteration_period_checker checker{period};

        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
    }

    SECTION("use simpler statements") {
        constexpr num_collect::index_type period = 2;
        iteration_period_checker checker{period};

        CHECK(checker);
        CHECK_FALSE(!checker);
        ++checker;
        CHECK_FALSE(checker);
        CHECK(!checker);
        ++checker;
        CHECK(checker);
        CHECK_FALSE(!checker);
        ++checker;
        CHECK_FALSE(checker);
        CHECK(!checker);
        ++checker;
        CHECK(checker);
        CHECK_FALSE(!checker);
    }

    SECTION("reset counter") {
        constexpr num_collect::index_type period = 3;
        iteration_period_checker checker{period};

        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());

        checker.reset();

        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK_FALSE(checker.is_start_of_period());
    }

    SECTION("set period to 1") {
        constexpr num_collect::index_type period = 1;
        iteration_period_checker checker{period};

        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
        checker.iterate();
        CHECK(checker.is_start_of_period());
    }

    SECTION("invalid periods") {
        CHECK_THROWS((void)iteration_period_checker(-1));
        CHECK_THROWS((void)iteration_period_checker(0));
        CHECK_NOTHROW((void)iteration_period_checker(1));
    }
}
