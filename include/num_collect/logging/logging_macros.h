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

#include "num_collect/logging/logger.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging {

/*!
 * \brief Write an error log and throw an exception for an error.
 *
 * \tparam Exception Type of the exception.
 * \tparam MessageArgs Types of arguments for formatting of the error message.
 * \param[in] source Information of the source code.
 * \param[in] l Logger.
 * \param[in] message Error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception,
    typename... MessageArgs>
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
 * \tparam MessageArgs Types of arguments for formatting of the error message.
 * \param[in] source Information of the source code.
 * \param[in] message Error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception,
    typename... MessageArgs>
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
