/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of exceptions in ode module.
 */
#include "num_collect/ode/ode_errors.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "num_collect/logging/logger.h"

TEST_CASE("NUM_COLLECT_ODE_THROW_LINEAR_SOLVER_FAILURE") {
    SECTION("throw an exception without formatting") {
        num_collect::logging::logger logger;
        try {
            NUM_COLLECT_ODE_THROW_LINEAR_SOLVER_FAILURE(
                logger, "Test message.");
            FAIL();
        } catch (const num_collect::ode::linear_solver_failure& exception) {
            CHECK_THAT(std::string(exception.what()),
                Catch::Matchers::StartsWith("Test message."));
        }
    }

    SECTION("throw an exception with formatting") {
        num_collect::logging::logger logger;
        try {
            NUM_COLLECT_ODE_THROW_LINEAR_SOLVER_FAILURE(
                logger, "Test message: {}.", 123);
            FAIL();
        } catch (const num_collect::ode::linear_solver_failure& exception) {
            CHECK_THAT(std::string(exception.what()),
                Catch::Matchers::StartsWith("Test message: 123."));
        }
    }
}
