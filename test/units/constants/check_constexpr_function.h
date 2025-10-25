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
 * \brief Definition of CHECK_CONSTEXPR_FUNCTION macro.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include <catch2/catch_test_macros.hpp>        // IWYU pragma: keep
#include <catch2/matchers/catch_matchers.hpp>  // IWYU pragma: keep
#include <catch2/matchers/catch_matchers_floating_point.hpp>  // IWYU pragma: keep

/*!
 * \brief Macro to check constexpr function by comparing with reference function
 * using relative error.
 *
 * \param ARGS Arguments to be passed to the functions enclosed in parentheses.
 * \param TESTED_FUNCTION Function to be tested.
 * \param REFERENCE_FUNCTION Function to calculate reference values.
 */
#define CHECK_CONSTEXPR_FUNCTION_RELATIVE(                                  \
    ARGS, TESTED_FUNCTION, REFERENCE_FUNCTION)                              \
    do {                                                                    \
        constexpr auto result_at_compile_time = TESTED_FUNCTION ARGS;       \
        using result_type = std::decay_t<decltype(result_at_compile_time)>; \
        const result_type result_runtime = TESTED_FUNCTION ARGS;            \
        const result_type reference_result =                                \
            static_cast<result_type>(REFERENCE_FUNCTION ARGS);              \
        CHECK_THAT(result_at_compile_time,                                  \
            Catch::Matchers::WithinRel(reference_result));                  \
        CHECK_THAT(                                                         \
            result_runtime, Catch::Matchers::WithinRel(reference_result));  \
    } while (false)

/*!
 * \brief Macro to check constexpr function by comparing with reference function
 * using absolute error.
 *
 * \param ARGS Arguments to be passed to the functions enclosed in parentheses.
 * \param TESTED_FUNCTION Function to be tested.
 * \param REFERENCE_FUNCTION Function to calculate reference values.
 */
#define CHECK_CONSTEXPR_FUNCTION_ABSOLUTE(                                     \
    ARGS, TESTED_FUNCTION, REFERENCE_FUNCTION)                                 \
    do {                                                                       \
        constexpr auto result_at_compile_time = TESTED_FUNCTION ARGS;          \
        using result_type = std::decay_t<decltype(result_at_compile_time)>;    \
        const result_type result_runtime = TESTED_FUNCTION ARGS;               \
        const result_type reference_result =                                   \
            static_cast<result_type>(REFERENCE_FUNCTION ARGS);                 \
        constexpr auto epsilon = std::numeric_limits<result_type>::epsilon() * \
            static_cast<result_type>(1e+2);                                    \
        CHECK_THAT(result_at_compile_time,                                     \
            Catch::Matchers::WithinAbs(reference_result, epsilon));            \
        CHECK_THAT(result_runtime,                                             \
            Catch::Matchers::WithinAbs(reference_result, epsilon));            \
    } while (false)

/*!
 * \brief Macro to check constexpr function by comparing with reference function
 * using equality.
 *
 * \param ARGS Arguments to be passed to the functions enclosed in parentheses.
 * \param TESTED_FUNCTION Function to be tested.
 * \param REFERENCE_FUNCTION Function to calculate reference values.
 */
#define CHECK_CONSTEXPR_FUNCTION_EQUAL(                                     \
    ARGS, TESTED_FUNCTION, REFERENCE_FUNCTION)                              \
    do {                                                                    \
        constexpr auto result_at_compile_time = TESTED_FUNCTION ARGS;       \
        using result_type = std::decay_t<decltype(result_at_compile_time)>; \
        const result_type result_runtime = TESTED_FUNCTION ARGS;            \
        const result_type reference_result =                                \
            static_cast<result_type>(REFERENCE_FUNCTION ARGS);              \
        CHECK(result_at_compile_time == reference_result);                  \
        CHECK(result_runtime == reference_result);                          \
    } while (false)
