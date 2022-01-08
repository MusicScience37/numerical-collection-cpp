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
 * \brief Definition of log_sink_base class.
 */
#pragma once

#include <iterator>
#include <string_view>
#include <utility>

#include <fmt/format.h>

#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging {

/*!
 * \brief Proxy class to write logs.
 *
 * \warning This class is assumed to be used only as an temporary object
 * returned from logger class.
 */
class logging_proxy {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] sink Log sink.
     * \param[in] write_log Whether to write log.
     */
    logging_proxy(std::string_view tag, log_level level,
        source_info_view source, log_sink_base* sink, bool write_log) noexcept
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

        write(body);
    }

    /*!
     * \brief Write a log.
     *
     * \tparam Args Type of arguments.
     * \param[in] format Format string.
     * \param[in] args Arguments for the format string.
     */
    template <typename... Args>
    void operator()(fmt::format_string<Args...> format, Args&&... args) const {
        if (!write_log_) {
            return;
        }

        fmt::memory_buffer buffer;
        fmt::format_to(
            std::back_inserter(buffer), format, std::forward<Args>(args)...);
        write(std::string_view(buffer.data(), buffer.size()));
    }

private:
    /*!
     * \brief Write a log.
     *
     * \param[in] body Body.
     */
    void write(std::string_view body) const {
        sink_->write(
            std::chrono::system_clock::now(), tag_, level_, source_, body);
    }

    //! Tag.
    std::string_view tag_;

    //! Log level.
    log_level level_;

    //! Information of the source code.
    source_info_view source_;

    //! Log sink.
    log_sink_base* sink_;

    //! Whether to write log.
    bool write_log_;
};

/*!
 * \brief Default log tag.
 */
inline constexpr auto default_tag = log_tag_view("");

/*!
 * \brief Class of loggers.
 */
class logger {
public:
    /*!
     * \brief Construct.
     */
    logger() : logger(default_tag) {}

    /*!
     * \brief Construct.
     *
     * \param[in] tag Tag.
     */
    explicit logger(log_tag_view tag)
        : logger(tag, log_config::instance().get_config_of(tag)) {}

    /*!
     * \brief Construct.
     *
     * \param[in] tag Tag.
     * \param[in] config Configuration.
     */
    logger(log_tag_view tag, log_tag_config config)
        : logger(log_tag(tag), std::move(config)) {}

    /*!
     * \brief Construct.
     *
     * \param[in] tag Tag.
     * \param[in] config Configuration.
     */
    logger(log_tag tag, log_tag_config config) noexcept
        : tag_(std::move(tag)), config_(std::move(config)) {}

    /*!
     * \brief Get the configuration.
     *
     * \return Configuration.
     */
    [[nodiscard]] auto config() const noexcept -> const log_tag_config& {
        return config_;
    }

    /*!
     * \brief Write a tarace log.
     *
     * \param[in] source Information of the source code.
     * \return Proxy object to write log.
     *
     * \note Argument source should be left to be the default value if you want
     * to write logs with the current position.
     */
    [[nodiscard]] auto trace(
        source_info_view source = source_info_view()) const noexcept
        -> logging_proxy {
        return logging_proxy(tag_.name(), log_level::trace, source,
            config_.sink().get(), config_.write_traces());
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
        source_info_view source = source_info_view()) const noexcept
        -> logging_proxy {
        return logging_proxy(tag_.name(), log_level::iteration, source,
            config_.sink().get(), config_.write_iterations());
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
        source_info_view source = source_info_view()) const noexcept
        -> logging_proxy {
        return logging_proxy(tag_.name(), log_level::iteration_label, source,
            config_.sink().get(), config_.write_iterations());
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
        source_info_view source = source_info_view()) const noexcept
        -> logging_proxy {
        return logging_proxy(tag_.name(), log_level::summary, source,
            config_.sink().get(), config_.write_summary());
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
        source_info_view source = source_info_view()) const noexcept
        -> logging_proxy {
        constexpr bool write_log = true;
        return logging_proxy(tag_.name(), log_level::warning, source,
            config_.sink().get(), write_log);
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
        source_info_view source = source_info_view()) const noexcept
        -> logging_proxy {
        constexpr bool write_log = true;
        return logging_proxy(tag_.name(), log_level::error, source,
            config_.sink().get(), write_log);
    }

private:
    //! Tag.
    log_tag tag_;

    //! Configuration.
    log_tag_config config_;
};

}  // namespace num_collect::logging
