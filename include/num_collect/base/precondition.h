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
 * \brief Definition of NUM_COLLECT_PRECONDITION macro.
 */
#pragma once

#include <iterator>
#include <string_view>
#include <utility>

#include <fmt/base.h>
#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect {
inline namespace base {
namespace impl {

/*!
 * \brief Handle a failure of a precondition.
 *
 * \param[in] source Information of the source code.
 * \param[in] condition String expression of the condition.
 * \param[in] logger Logger.
 * \param[in] description Description of the precondition.
 */
[[noreturn]] inline void handle_precondition_failure(
    util::source_info_view source, std::string_view condition,
    const logging::logger& logger, std::string_view description) {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer),
        "Precondition failed: {} (Condition: {})", description, condition);
    logging::log_and_throw<precondition_not_satisfied>(
        source, logger, std::string_view(buffer.data(), buffer.size()));
}

/*!
 * \brief Handle a failure of a precondition.
 *
 * \param[in] source Information of the source code.
 * \param[in] condition String expression of the condition.
 * \param[in] description Description of the precondition.
 */
[[noreturn]] inline void handle_precondition_failure(
    util::source_info_view source, std::string_view condition,
    std::string_view description) {
    logging::logger logger;
    handle_precondition_failure(source, condition, logger, description);
}

/*!
 * \brief Handle a failure of a precondition.
 *
 * \tparam Args Types of arguments of the format string of the description.
 * \param[in] source Information of the source code.
 * \param[in] condition String expression of the condition.
 * \param[in] logger Logger.
 * \param[in] description_format Format string of the description.
 * \param[in] description_args Arguments for the format string of the
 * description.
 */
template <typename... Args>
    requires(sizeof...(Args) > 0)
[[noreturn]] inline void handle_precondition_failure(
    util::source_info_view source, std::string_view condition,
    const logging::logger& logger,
    fmt::format_string<Args...> description_format,
    Args&&... description_args) {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), description_format,
        std::forward<Args>(description_args)...);
    handle_precondition_failure(source, condition, logger,
        std::string_view(buffer.data(), buffer.size()));
}

/*!
 * \brief Handle a failure of a precondition.
 *
 * \tparam Args Types of arguments of the format string of the description.
 * \param[in] source Information of the source code.
 * \param[in] condition String expression of the condition.
 * \param[in] description_format Format string of the description.
 * \param[in] description_args Arguments for the format string of the
 * description.
 */
template <typename... Args>
    requires(sizeof...(Args) > 0)
[[noreturn]] inline void handle_precondition_failure(
    util::source_info_view source, std::string_view condition,
    fmt::format_string<Args...> description_format,
    Args&&... description_args) {
    logging::logger logger;
    handle_precondition_failure(source, condition, logger, description_format,
        std::forward<Args>(description_args)...);
}

}  // namespace impl
}  // namespace base
}  // namespace num_collect

// clang-format off
/*!
 * \brief Check whether a precondition is satisfied and throw an exception if
 * not.
 *
 * This macro can be usable as one of the following signatures:
 *
 * - `NUM_COLLECT_PRECONDITION(CONDITION, LOGGER, DESCRIPTION)`
 * - `NUM_COLLECT_PRECONDITION(CONDITION, DESCRIPTION)`
 * - `NUM_COLLECT_PRECONDITION(CONDITION, LOGGER, DESCRIPTION_FORMAT, DESCRIPTION_ARGS...)`
 * - `NUM_COLLECT_PRECONDITION(CONDITION, DESCRIPTION_FORMAT, DESCRIPTION_ARGS...)`
 *
 * Arguments are as follows:
 *
 * - `CONDITION`: Condition.
 * - `LOGGER`: num_collect::logging::logger object to write the log.
 * - `DESCRIPTION`: Description of the precondition.
 * - `DESCRIPTION_FORMAT`: Format string of the description.
 * - `DESCRIPTION_ARGS...`: Arguments for the format string of the description.
 */
// clang-format on
#define NUM_COLLECT_PRECONDITION(CONDITION, ...) /* NOLINT */        \
    do {                                                             \
        if (!(CONDITION)) [[unlikely]] {                             \
            ::num_collect::base::impl::handle_precondition_failure(  \
                ::num_collect::util::source_info_view(), #CONDITION, \
                __VA_ARGS__);                                        \
        }                                                            \
    } while (false)
