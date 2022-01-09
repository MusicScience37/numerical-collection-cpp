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

#include <iterator>
#include <string_view>

#include <fmt/format.h>

#include "num_collect/logging/log_and_throw.h"
#include "num_collect/util/exception.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::impl {

/*!
 * \brief Check whether a condition is satisfied.
 *
 * \param[in] condition_result Result of the condition.
 * \param[in] condition_str String expression of the condition.
 * \param[in] source Information of the source code.
 */
template <typename ConditionResult>
void assert_impl(ConditionResult&& condition_result,
    std::string_view condition_str,
    source_info_view source = source_info_view()) {
    if (!std::forward<ConditionResult>(condition_result)) {
        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer),
            FMT_STRING("Assertion failure: {}"), condition_str);
        logging::log_and_throw<assertion_failure>(
            std::string_view(buffer.data(), buffer.size()), source);
    }
}

}  // namespace num_collect::impl

#ifdef NUM_COLLECT_DOCUMENTATION
/*!
 * \brief Macro to check whether a condition is satisfied.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_ASSERT(CONDITION) \
    ::num_collect::impl::assert_impl((CONDITION), (#CONDITION))
#else
// NOLINTNEXTLINE
#define NUM_COLLECT_ASSERT(CONDITION) \
    ::num_collect::impl::assert_impl((CONDITION), (#CONDITION))
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
