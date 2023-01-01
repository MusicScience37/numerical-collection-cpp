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
 * \brief Definition of toml_single_file_log_sink_config_parser class.
 */
#pragma once

#include <memory>

#include <toml++/toml.h>

#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/config/toml/toml_log_sink_config_parser_base.h"
#include "num_collect/logging/config/toml/toml_single_file_log_sink_factory.h"

namespace num_collect::logging::config::toml {

/*!
 * \brief Class to parse configurations of log sinks to write to files without
 * rotation.
 *
 * \thread_safety Not thread-safe.
 */
class toml_single_file_log_sink_config_parser final
    : public toml_log_sink_config_parser_base {
public:
    //! \copydoc num_collect::logging::config::toml::toml_log_sink_config_parser_base::parse
    [[nodiscard]] auto parse(const ::toml::table& table)
        -> std::shared_ptr<log_sink_factory_base> override {
        return std::make_shared<toml_single_file_log_sink_factory>(table);
    }

    /*!
     * \brief Constructor.
     */
    toml_single_file_log_sink_config_parser() noexcept = default;

    /*!
     * \brief Destructor.
     */
    ~toml_single_file_log_sink_config_parser() noexcept override = default;

    toml_single_file_log_sink_config_parser(
        const toml_single_file_log_sink_config_parser&) = delete;
    toml_single_file_log_sink_config_parser(
        toml_single_file_log_sink_config_parser&&) = delete;
    auto operator=(const toml_single_file_log_sink_config_parser&)
        -> toml_single_file_log_sink_config_parser& = delete;
    auto operator=(toml_single_file_log_sink_config_parser&&)
        -> toml_single_file_log_sink_config_parser& = delete;
};

}  // namespace num_collect::logging::config::toml
