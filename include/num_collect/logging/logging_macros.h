/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of macros for logging.
 */
#pragma once

#include <concepts>
#include <iterator>
#include <string_view>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/logger.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging {

namespace impl {

/*!
 * \brief Write a log without check of the condition to write logs.
 *
 * \param[in] source Information of the source code.
 * \param[in] l Logger.
 * \param[in] level Log level.
 * \param[in] body Body.
 */
inline void log_without_condition_check(util::source_info_view source,
    const logger& l, log_level level, std::string_view body) {
    l.log_without_condition_check(source, level, body);
}

/*!
 * \brief Write a log without check of the condition to write logs.
 *
 * \tparam Args Type of arguments.
 * \param[in] source Information of the source code.
 * \param[in] l Logger.
 * \param[in] level Log level.
 * \param[in] format Format string.
 * \param[in] args Arguments for the format string.
 */
template <typename... Args>
    requires(sizeof...(Args) > 0)
inline void log_without_condition_check(util::source_info_view source,
    const logger& l, log_level level, fmt::format_string<Args...> format,
    Args&&... args) {
    l.log_without_condition_check(
        source, level, format, std::forward<Args>(args)...);
}

}  // namespace impl

/*!
 * \brief Write an error log and throw an exception for an error.
 *
 * \tparam Exception Type of the exception.
 * \param[in] source Information of the source code.
 * \param[in] l Logger.
 * \param[in] message Error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception>
[[noreturn]] void log_and_throw(
    util::source_info_view source, const logger& l, std::string_view message) {
    l.error(source)(message);
    throw Exception(message, source);
}

/*!
 * \brief Write an error log and throw an exception for an error.
 *
 * \tparam Exception Type of the exception.
 * \tparam MessageArgs Types of arguments for formatting of the error message.
 * \param[in] source Information of the source code.
 * \param[in] l Logger.
 * \param[in] message_format Format of the error message.
 * \param[in] message_args Arguments for formatting of the error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception,
    typename... MessageArgs>
    requires(sizeof...(MessageArgs) > 0)
[[noreturn]] void log_and_throw(util::source_info_view source, const logger& l,
    fmt::format_string<MessageArgs...> message_format,
    MessageArgs&&... message_args) {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), message_format,
        std::forward<MessageArgs>(message_args)...);
    log_and_throw<Exception>(
        source, l, std::string_view(buffer.data(), buffer.size()));
}

/*!
 * \brief Write an error log and throw an exception for an error.
 *
 * \tparam Exception Type of the exception.
 * \param[in] source Information of the source code.
 * \param[in] message Error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception>
[[noreturn]] void log_and_throw(
    util::source_info_view source, std::string_view message) {
    logger l;
    log_and_throw<Exception>(source, l, message);
}

/*!
 * \brief Write an error log and throw an exception for an error.
 *
 * \tparam Exception Type of the exception.
 * \tparam MessageArgs Types of arguments for formatting of the error message.
 * \param[in] source Information of the source code.
 * \param[in] message_format Format of the error message.
 * \param[in] message_args Arguments for formatting of the error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception,
    typename... MessageArgs>
    requires(sizeof...(MessageArgs) > 0)
[[noreturn]] void log_and_throw(util::source_info_view source,
    fmt::format_string<MessageArgs...> message_format,
    MessageArgs&&... message_args) {
    logger l;
    log_and_throw<Exception>(
        source, l, message_format, std::forward<MessageArgs>(message_args)...);
}

}  // namespace num_collect::logging

/*!
 * \brief Write a log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - `LEVEL`: Log level.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 */
#define INTERNAL_NUM_COLLECT_LOG_IMPL(LOGGER, LEVEL, ...) /*NOLINT*/    \
    do {                                                                \
        if (LOGGER.should_log(LEVEL)) {                                 \
            ::num_collect::logging::impl::log_without_condition_check(  \
                ::num_collect::util::source_info_view(), LOGGER, LEVEL, \
                __VA_ARGS__);                                           \
        }                                                               \
    } while (false)

/*!
 * \brief Write a trace log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_TRACE(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                    \
        LOGGER, ::num_collect::logging::log_level::trace, __VA_ARGS__)

/*!
 * \brief Write a debug log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_DEBUG(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                    \
        LOGGER, ::num_collect::logging::log_level::debug, __VA_ARGS__)

/*!
 * \brief Write a log of an iteration.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_ITERATION(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                        \
        LOGGER, ::num_collect::logging::log_level::iteration, __VA_ARGS__)

/*!
 * \brief Write a log of a label of iterations.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_ITERATION_LABEL(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(LOGGER,                       \
        ::num_collect::logging::log_level::iteration_label, __VA_ARGS__)

/*!
 * \brief Write a summary log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_SUMMARY(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                      \
        LOGGER, ::num_collect::logging::log_level::summary, __VA_ARGS__)

/*!
 * \brief Write an information log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_INFO(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                   \
        LOGGER, ::num_collect::logging::log_level::info, __VA_ARGS__)

/*!
 * \brief Write a warning log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_WARNING(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                      \
        LOGGER, ::num_collect::logging::log_level::warning, __VA_ARGS__)

/*!
 * \brief Write an error log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_ERROR(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                    \
        LOGGER, ::num_collect::logging::log_level::error, __VA_ARGS__)

/*!
 * \brief Write a critical log.
 *
 * Arguments:
 *
 * - `LOGGER`: Logger.
 * - Remaining arguments are either
 *   - the log body, or
 *   - the format string and its arguments of the log body.
 *
 * For log levels, see \see num_collect::logging::log_level.
 */
#define NUM_COLLECT_LOG_CRITICAL(LOGGER, ...) /*NOLINT*/ \
    INTERNAL_NUM_COLLECT_LOG_IMPL(                       \
        LOGGER, ::num_collect::logging::log_level::critical, __VA_ARGS__)

/*!
 * \brief Write an error log and throw an exception for an error.
 *
 * This macro can be usable as following signature:
 *
 * - `NUM_COLLECT_LOG_AND_THROW(EXCEPTION_TYPE, LOGGER, MESSAGE_FORMAT,
 * MESSAGE_ARGS...)`
 * - `NUM_COLLECT_LOG_AND_THROW(EXCEPTION_TYPE, MESSAGE_FORMAT,
 * MESSAGE_ARGS...)`
 *
 * Here arguments are as follows:
 *
 * - `EXCEPTION_TYPE`: Type of the exception.
 * - `LOGGER`: num_collect::logging::logger object to write the log.
 * - `MESSAGE_FORMAT`: Format of the error message.
 * - `MESSAGE_ARGS...`: Arguments for formatting of the error message.
 */
#define NUM_COLLECT_LOG_AND_THROW(EXCEPTION_TYPE, ...) /*NOLINT*/ \
    ::num_collect::logging::log_and_throw<EXCEPTION_TYPE>(        \
        ::num_collect::util::source_info_view(), __VA_ARGS__)
