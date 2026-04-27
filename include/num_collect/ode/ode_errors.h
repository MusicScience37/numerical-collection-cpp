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
 * \brief Definitions of classes of exceptions in ode module.
 */
#pragma once

#include "num_collect/base/exception.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/logger.h"

namespace num_collect::ode {

/*!
 * \brief Base class of exceptions in ode module.
 *
 * \note General errors like invalid arguments should be reported by existing
 * exceptions in base module.
 */
class ode_exception : public algorithm_failure {
public:
    using algorithm_failure::algorithm_failure;
};

/*!
 * \brief Class of exceptions for failures in linear solvers.
 *
 * \note This type of errors can be sometimes recoverable by reducing step size,
 * so a specific type is defined for exceptions of this type.
 */
class linear_solver_failure : public ode_exception {
public:
    using ode_exception::ode_exception;
};

namespace impl {

/*!
 * \brief Write a log and throw an exception for an error in ode module.
 *
 * \tparam Exception Type of the exception.
 * \tparam MessageArgs Types of arguments for formatting of the error message.
 * \param[in] source Information of the source code.
 * \param[in] level Log level.
 * \param[in] logger Logger.
 * \param[in] message Error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception>
[[noreturn]] void log_and_throw(util::source_info_view source,
    logging::log_level level, const logging::logger& logger,
    std::string_view message) {
    logger.log(level, source)(message);
    throw Exception(message, source);
}

/*!
 * \brief Write a log and throw an exception for an error in ode module.
 *
 * \tparam Exception Type of the exception.
 * \tparam MessageArgs Types of arguments for formatting of the error message.
 * \param[in] source Information of the source code.
 * \param[in] level Log level.
 * \param[in] logger Logger.
 * \param[in] message_format Format of the error message.
 * \param[in] message_args Arguments for formatting of the error message.
 */
template <
    std::constructible_from<std::string_view, util::source_info_view> Exception,
    typename... MessageArgs>
    requires(sizeof...(MessageArgs) > 0)
[[noreturn]] void log_and_throw(util::source_info_view source,
    logging::log_level level, const logging::logger& logger,
    fmt::format_string<MessageArgs...> message_format,
    MessageArgs&&... message_args) {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), message_format,
        std::forward<MessageArgs>(message_args)...);
    log_and_throw<Exception>(
        source, level, logger, std::string_view(buffer.data(), buffer.size()));
}

}  // namespace impl

}  // namespace num_collect::ode

/*!
 * \brief Write a log and throw an exception for an error in ode module.
 *
 * \param[in] EXCEPTION_TYPE Type of the exception.
 * \param[in] LEVEL Log level.
 * \param[in] LOGGER Logger.
 *
 * Remaining arguments are the format string and its arguments of the error
 * message.
 */
#define INTERNAL_NUM_COLLECT_ODE_LOG_AND_THROW(              \
    EXCEPTION_TYPE, LEVEL, LOGGER, ...)                      \
    ::num_collect::ode::impl::log_and_throw<EXCEPTION_TYPE>( \
        ::num_collect::util::source_info_view(), LEVEL, LOGGER, __VA_ARGS__)

/*!
 * \brief Throw an exception for a failure in linear solvers with a log.
 *
 * \param[in] LOGGER Logger.
 *
 * Remaining arguments are the format string and its arguments of the error
 * message.
 */
#define NUM_COLLECT_ODE_THROW_LINEAR_SOLVER_FAILURE(LOGGER, ...) \
    INTERNAL_NUM_COLLECT_ODE_LOG_AND_THROW(                      \
        ::num_collect::ode::linear_solver_failure,               \
        ::num_collect::logging::log_level::debug, LOGGER, __VA_ARGS__)
