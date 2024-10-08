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
 * \brief Definition of logger class.
 */
#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/base.h>
#include <fmt/format.h>

#include "num_collect/logging/impl/iteration_layer_handler.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/sinks/log_sink.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging {

/*!
 * \brief Proxy class to write logs.
 *
 * \warning This class is assumed to be used only as an temporary object
 * returned from num_collect::logging::logger class.
 */
class logging_proxy {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] sink Log sink.
     * \param[in] write_log Whether to write log.
     */
    logging_proxy(std::string_view tag, log_level level,
        util::source_info_view source, const sinks::log_sink* sink,
        bool write_log) noexcept
        : tag_(tag),
          level_(level),
          source_(source),
          sink_(sink),
          write_log_(write_log) {}

    /*!
     * \brief Write a log.
     *
     * \param[in] body Body.
     */
    void operator()(std::string_view body) const {
        if (!write_log_) {
            return;
        }

        sink_->write(time_stamp::now(), tag_, level_, source_, body);
    }

    /*!
     * \brief Write a log.
     *
     * \tparam Args Type of arguments.
     * \param[in] format Format string.
     * \param[in] args Arguments for the format string.
     */
    template <typename... Args>
        requires(sizeof...(Args) > 0)
    void operator()(fmt::format_string<Args...> format, Args&&... args) const {
        if (!write_log_) {
            return;
        }

        fmt::memory_buffer buffer;
        fmt::format_to(
            std::back_inserter(buffer), format, std::forward<Args>(args)...);
        sink_->write(time_stamp::now(), tag_, level_, source_,
            std::string_view{buffer.data(), buffer.size()});
    }

private:
    //! Tag.
    std::string_view tag_;

    //! Log level.
    log_level level_;

    //! Information of the source code.
    util::source_info_view source_;

    //! Log sink.
    const sinks::log_sink* sink_;

    //! Whether to write log.
    bool write_log_;
};

/*!
 * \brief Default log tag.
 */
constexpr auto default_tag = log_tag_view("");

/*!
 * \brief Class of loggers.
 *
 * \thread_safety All `const` member functions (member functions except for
 * initialize_child_algorithm_logger function, constructors, destructor, and
 * assignment operators) are thread safe even for the same object.
 *
 * For logging, following macros can be used:
 *
 * - \ref NUM_COLLECT_LOG_TRACE
 * - \ref NUM_COLLECT_LOG_DEBUG
 * - \ref NUM_COLLECT_LOG_ITERATION
 * - \ref NUM_COLLECT_LOG_ITERATION_LABEL
 * - \ref NUM_COLLECT_LOG_SUMMARY
 * - \ref NUM_COLLECT_LOG_INFO
 * - \ref NUM_COLLECT_LOG_WARNING
 * - \ref NUM_COLLECT_LOG_ERROR
 * - \ref NUM_COLLECT_LOG_CRITICAL
 * - \ref NUM_COLLECT_LOG_AND_THROW
 */
class logger {
public:
    /*!
     * \brief Constructor.
     */
    logger() : logger(default_tag) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] tag Tag.
     */
    explicit logger(log_tag_view tag) : logger(tag, get_config_of(tag)) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] tag Tag.
     * \param[in] config Configuration.
     */
    logger(log_tag_view tag, log_tag_config config)
        : logger(static_cast<log_tag>(tag), std::move(config)) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] tag Tag.
     * \param[in] config Configuration.
     */
    logger(log_tag tag, log_tag_config config)
        : tag_(std::move(tag)),
          config_(std::move(config)),
          always_output_log_level_(std::max(config_.output_log_level(),
              config_.output_log_level_in_child_iterations())),
          lowest_output_log_level_(std::min(config_.output_log_level(),
              config_.output_log_level_in_child_iterations())) {}

    /*!
     * \brief Get the log tag.
     *
     * \return Log tag.
     */
    [[nodiscard]] auto tag() const noexcept -> const log_tag& { return tag_; }

    /*!
     * \brief Get the configuration.
     *
     * \return Configuration.
     */
    [[nodiscard]] auto config() const noexcept -> const log_tag_config& {
        return config_;
    }

    /*!
     * \brief Set this node to an iterative algorithm.
     */
    void set_iterative() const noexcept {
        iteration_layer_handler_.set_iterative();
    }

    /*!
     * \brief Initialize a logger as the logger of the algorithm called from the
     * algorithm of this logger.
     *
     * \param[in] child Logger of the algorithm called from the algorithm of
     * this logger.
     */
    void initialize_child_algorithm_logger(logger& child) noexcept {
        iteration_layer_handler_.initialize_lower_layer(
            child.iteration_layer_handler_);
    }

    /*!
     * \brief Check whether to write logs with a log level.
     *
     * \param[in] level Log level.
     * \retval true Should write logs.
     * \retval false Should not write logs.
     */
    [[nodiscard]] auto should_log(log_level level) const noexcept -> bool {
        if (level < lowest_output_log_level_) {
            return false;
        }
        if (level >= always_output_log_level_) {
            return true;
        }
        if (iteration_layer_handler_.is_upper_layer_iterative()) {
            return level >= config_.output_log_level_in_child_iterations();
        }
        return level >= config_.output_log_level();
    }

    /*!
     * \brief Write a log.
     *
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto log(log_level level,
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return logging_proxy(
            tag_.name(), level, source, &config_.sink(), should_log(level));
    }

    /*!
     * \brief Write a trace log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto trace(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::trace, source);
    }

    /*!
     * \brief Write a debug log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto debug(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::debug, source);
    }

    /*!
     * \brief Write a iteration log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     * \note This class don't take configurations of periods into account.
     */
    [[nodiscard]] auto iteration(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::iteration, source);
    }

    /*!
     * \brief Write a label of iteration logs.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     * \note This class don't take configurations of periods into account.
     */
    [[nodiscard]] auto iteration_label(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::iteration_label, source);
    }

    /*!
     * \brief Write a summary log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     * \note This class don't take configurations of periods into account.
     */
    [[nodiscard]] auto summary(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::summary, source);
    }

    /*!
     * \brief Write a information log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto info(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::info, source);
    }

    /*!
     * \brief Write a warning log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto warning(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::warning, source);
    }

    /*!
     * \brief Write a error log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto error(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::error, source);
    }

    /*!
     * \brief Write a critical log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto critical(
        util::source_info_view source = util::source_info_view()) const noexcept
        -> logging_proxy {
        return log(log_level::critical, source);
    }

    /*!
     * \brief Write a log without check of the condition to write logs.
     *
     * \warning This function should be used with should_log() function.
     *
     * \param[in] source Information of the source code.
     * \param[in] level Log level.
     * \param[in] body Body.
     */
    void log_without_condition_check(util::source_info_view source,
        log_level level, std::string_view body) const {
        config_.sink().write(
            time_stamp::now(), tag_.name(), level, source, body);
    }

    /*!
     * \brief Write a log without check of the condition to write logs.
     *
     * \warning This function should be used with should_log() function.
     *
     * \tparam Args Type of arguments.
     * \param[in] source Information of the source code.
     * \param[in] level Log level.
     * \param[in] format Format string.
     * \param[in] args Arguments for the format string.
     */
    template <typename... Args>
        requires(sizeof...(Args) > 0)
    void log_without_condition_check(util::source_info_view source,
        log_level level, fmt::format_string<Args...> format,
        Args&&... args) const {
        fmt::memory_buffer buffer;
        fmt::format_to(
            std::back_inserter(buffer), format, std::forward<Args>(args)...);
        config_.sink().write(time_stamp::now(), tag_.name(), level, source,
            std::string_view(buffer.data(), buffer.size()));
    }

private:
    //! Tag.
    log_tag tag_;

    //! Configuration.
    log_tag_config config_;

    //! Minimum log level to output always.
    log_level always_output_log_level_;

    //! Lowest log level to output.
    log_level lowest_output_log_level_;

    //! Handler of layers of iterations.
    impl::iteration_layer_handler iteration_layer_handler_{};
};

}  // namespace num_collect::logging
