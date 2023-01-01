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
 * \brief Definition of toml_log_sink_config_parser_base class.
 */
#pragma once

#include <memory>

#include <toml++/toml.h>

#include "num_collect/logging/config/log_sink_factory_base.h"

namespace num_collect::logging::config::toml {

/*!
 * \brief Interface of parser of configurations of log sinks.
 */
class toml_log_sink_config_parser_base {
public:
    /*!
     * \brief Parse a configuration of a log sink.
     *
     * \param[in] table Table.
     * \return Factory of the log sink.
     */
    [[nodiscard]] virtual auto parse(const ::toml::table& table)
        -> std::shared_ptr<log_sink_factory_base> = 0;

    toml_log_sink_config_parser_base(
        const toml_log_sink_config_parser_base&) = delete;
    toml_log_sink_config_parser_base(
        toml_log_sink_config_parser_base&&) = delete;
    auto operator=(const toml_log_sink_config_parser_base&)
        -> toml_log_sink_config_parser_base& = delete;
    auto operator=(toml_log_sink_config_parser_base&&)
        -> toml_log_sink_config_parser_base& = delete;

    /*!
     * \brief Destructor.
     */
    virtual ~toml_log_sink_config_parser_base() noexcept = default;

protected:
    /*!
     * \brief Constructor.
     */
    toml_log_sink_config_parser_base() noexcept = default;
};

}  // namespace num_collect::logging::config::toml
