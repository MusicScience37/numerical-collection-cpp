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
 * \brief Definition of assertion macros.
 */
#pragma once

#include <fmt/core.h>

#include "num_collect/util/exception.h"

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief macro to get function name
 *
 * This macro will be expanded to implementation-defined macros or variables.
 */
#define NUM_COLLECT_FUNCTION <implementation defined strings>
#elif __GNUC__                                    // GCC and Clang
#define NUM_COLLECT_FUNCTION __PRETTY_FUNCTION__  // NOLINT
#elif _MSC_VER                                    // MSVC
#define NUM_COLLECT_FUNCTION __FUNCSIG__          // NOLINT
#else                                             // fallback (C++ standard)
#define NUM_COLLECT_FUNCTION __func__             // NOLINT
#endif

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief Macro to check whether a condition is satisfied.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_ASSERT(CONDITION)                                    \
    [&] {                                                                \
        if (!(CONDITION)) {                                              \
            throw ::num_collect::assertion_failure(                      \
                fmt::format("assertion failure: {} (at {})", #CONDITION, \
                    NUM_COLLECT_FUNCTION));                              \
        }                                                                \
    }()
#else
// NOLINTNEXTLINE
#define NUM_COLLECT_ASSERT(CONDITION)                                    \
    [&] {                                                                \
        if (!(CONDITION)) {                                              \
            throw ::num_collect::assertion_failure(                      \
                fmt::format("assertion failure: {} (at {})", #CONDITION, \
                    NUM_COLLECT_FUNCTION));                              \
        }                                                                \
    }()
#endif

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief Macro to check whether a condition is satisfied in debug build only.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_DEBUG_ASSERT(CONDITION) NUM_COLLECT_ASSERT(CONDITION)
#elif !defined(NDEBUG)
// NOLINTNEXTLINE
#define NUM_COLLECT_DEBUG_ASSERT(CONDITION) NUM_COLLECT_ASSERT(CONDITION)
#else
// NOLINTNEXTLINE
#define NUM_COLLECT_DEBUG_ASSERT(CONDITION) [] {}()
#endif
