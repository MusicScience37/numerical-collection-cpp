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
 * \brief Test of NUM_COLLECT_PRECONDITION macro.
 */
#include "num_collect/base/precondition.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"

TEST_CASE("NUM_COLLECT_PRECONDITION") {
    num_collect::logging::logger logger{
        num_collect::logging::log_tag{"num_collect_test::base::preconditions"}};

    SECTION("use with a condition and a description") {
        SECTION("when the condition is true") {
            NUM_COLLECT_PRECONDITION(1 == 1, "Test of preconditions.");
        }
        SECTION("when the condition is false") {
            try {
                NUM_COLLECT_PRECONDITION(1 == 2, "Test of preconditions.");
                FAIL("The precondition should throw an exception.");
            } catch (const num_collect::precondition_not_satisfied& e) {
                CHECK_THAT(e.what(),
                    Catch::Matchers::ContainsSubstring(
                        "Precondition failed: Test of preconditions. "
                        "(Condition: 1 == 2)"));
            }
        }
    }

    SECTION("use with a condition, a logger, and a description") {
        SECTION("when the condition is true") {
            NUM_COLLECT_PRECONDITION(1 == 1, logger, "Test of preconditions.");
        }
        SECTION("when the condition is false") {
            try {
                NUM_COLLECT_PRECONDITION(
                    1 == 2, logger, "Test of preconditions.");
                FAIL("The precondition should throw an exception.");
            } catch (const num_collect::precondition_not_satisfied& e) {
                CHECK_THAT(e.what(),
                    Catch::Matchers::ContainsSubstring(
                        "Precondition failed: Test of preconditions. "
                        "(Condition: 1 == 2)"));
            }
        }
    }

    SECTION("use with a condition, a logger, and a formatted description") {
        SECTION("when the condition is true") {
            NUM_COLLECT_PRECONDITION(
                1 == 1, logger, "Test of preconditions with parameter: {}.", 1);
        }
        SECTION("when the condition is false") {
            try {
                NUM_COLLECT_PRECONDITION(1 == 2, logger,
                    "Test of preconditions with parameter: {}.", 1);
                FAIL("The precondition should throw an exception.");
            } catch (const num_collect::precondition_not_satisfied& e) {
                CHECK_THAT(e.what(),
                    Catch::Matchers::ContainsSubstring(
                        "Precondition failed: Test of preconditions with "
                        "parameter: 1. "
                        "(Condition: 1 == 2)"));
            }
        }
    }

    SECTION("use with a condition, and a formatted description") {
        SECTION("when the condition is true") {
            NUM_COLLECT_PRECONDITION(
                1 == 1, "Test of preconditions with parameter: {}.", 1);
        }
        SECTION("when the condition is false") {
            try {
                NUM_COLLECT_PRECONDITION(
                    1 == 2, "Test of preconditions with parameter: {}.", 1);
                FAIL("The precondition should throw an exception.");
            } catch (const num_collect::precondition_not_satisfied& e) {
                CHECK_THAT(e.what(),
                    Catch::Matchers::ContainsSubstring(
                        "Precondition failed: Test of preconditions with "
                        "parameter: 1. "
                        "(Condition: 1 == 2)"));
            }
        }
    }
}
