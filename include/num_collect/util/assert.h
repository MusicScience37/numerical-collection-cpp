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
 * \brief Definition of assertion macros.
 */
#pragma once

#include <cstdio>
#include <cstdlib>
#include <string_view>

#include <fmt/base.h>
#include <fmt/format.h>

#include "num_collect/util/source_info_view.h"

namespace num_collect::util::impl {

/*!
 * \brief Handle a failure of an assertion.
 *
 * \param[in] condition_str Source code of the condition.
 * \param[in] source Information of the source code.
 *
 * This function never returns or throws exceptions,
 * and calls `std::abort()` after printing a message to standard error output.
 */
[[noreturn]] inline void handle_assertion_failure(
    std::string_view condition_str,
    source_info_view source = source_info_view()) noexcept {
    try {
        fmt::print(stderr, "Assertion failed at {}:{} ({}): {}\n",
            source.file_path(), source.line(), source.function_name(),
            condition_str);
    } catch (...) {  // NOLINT(*-empty-catch)
        // Nothing can be done here.
    }
    std::abort();
}

}  // namespace num_collect::util::impl

/*!
 * \brief Macro to check whether a condition is satisfied.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_ASSERT_IMPL(CONDITION)                                     \
    do {                                                                       \
        if (!(CONDITION)) [[unlikely]] {                                       \
            ::num_collect::util::impl::handle_assertion_failure((#CONDITION)); \
        }                                                                      \
    } while (false)

/*!
 * \brief Macro to check whether a condition is satisfied.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_ASSERT(CONDITION) NUM_COLLECT_ASSERT_IMPL(CONDITION)

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief Macro to check whether a condition is satisfied in debug build only.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_DEBUG_ASSERT(CONDITION) NUM_COLLECT_ASSERT(CONDITION)
#elif !defined(NDEBUG)
/*!
 * \brief Macro to check whether a condition is satisfied in debug build only.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_DEBUG_ASSERT(CONDITION) NUM_COLLECT_ASSERT(CONDITION)
#else
/*!
 * \brief Macro to check whether a condition is satisfied in debug build only.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_DEBUG_ASSERT(CONDITION) [] {}()
#endif
