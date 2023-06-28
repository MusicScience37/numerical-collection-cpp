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
 * \brief Definition of toml_log_config_parser class.
 */
#pragma once

#include <exception>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>
#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/config/log_config_parser_base.h"
#include "num_collect/logging/config/log_sink_factory_table.h"
#include "num_collect/logging/config/parse_output_log_level_str.h"
#include "num_collect/logging/config/toml/toml_async_log_sink_config_parser.h"
#include "num_collect/logging/config/toml/toml_combined_log_sink_config_parser.h"
#include "num_collect/logging/config/toml/toml_console_log_sink_config_parser.h"
#include "num_collect/logging/config/toml/toml_helper.h"
#include "num_collect/logging/config/toml/toml_log_sink_config_parser_base.h"
#include "num_collect/logging/config/toml/toml_single_file_log_sink_config_parser.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"

namespace num_collect::logging::config::toml {

namespace impl {

/*!
 * \brief Get a log level value or throw exception.
 *
 * \param[in] table Table in toml++ library.
 * \param[in] path Path.
 * \param[in] config_name Configuration name for exceptions.
 * \return Value.
 */
inline auto require_log_level(const ::toml::table& table, std::string_view path,
    std::string_view config_name) -> log_level {
    const auto str =
        require_value<std::string>(table, path, config_name, "a string");
    return parse_output_log_level_str(str);
}

/*!
 * \brief Parse configurations of logging for a log tag.
 *
 * \param[out] config Configuration.
 * \param[in] table Table in toml++ library.
 * \param[in] sinks Log sinks.
 */
inline void parse_log_tag_config_to(log_tag_config& config,
    const ::toml::table& table, log_sink_factory_table& sinks) {
    if (table.contains("sink")) {
        const auto sink_name = require_value<std::string>(table, "sink",
            "sink in num_collect.logging.tag_configs element", "a string");
        config.sink(sinks.get(sink_name));
    }

    if (table.contains("output_log_level")) {
        const auto level = require_log_level(table, "output_log_level",
            "output_log_level in num_collect.logging.tag_configs element");
        config.output_log_level(level);
    }

    if (table.contains("output_log_level_in_child_iterations")) {
        const auto level =
            require_log_level(table, "output_log_level_in_child_iterations",
                "output_log_level_in_child_iterations in "
                "num_collect.logging.tag_configs element");
        config.output_log_level_in_child_iterations(level);
    }

    if (table.contains("iteration_output_period")) {
        const auto val =
            require_value<index_type>(table, "iteration_output_period",
                "iteration_output_period in num_collect.logging.tag_configs "
                "element",
                "an integer");
        config.iteration_output_period(val);
    }

    if (table.contains("iteration_label_period")) {
        const auto val =
            require_value<index_type>(table, "iteration_label_period",
                "iteration_label_period in num_collect.logging.tag_configs "
                "element",
                "an integer");
        config.iteration_label_period(val);
    }
}

}  // namespace impl

/*!
 * \brief Class to parse log configuration from a TOML file.
 *
 * \thread_safety Not thread-safe.
 */
class toml_log_config_parser final : public log_config_parser_base {
public:
    /*!
     * \brief Constructor.
     */
    toml_log_config_parser() {
        append_log_sink_config_parser(
            "console", std::make_shared<toml_console_log_sink_config_parser>());
        append_log_sink_config_parser("single_file",
            std::make_shared<toml_single_file_log_sink_config_parser>());
        append_log_sink_config_parser(
            "async", std::make_shared<toml_async_log_sink_config_parser>());
        append_log_sink_config_parser("combined",
            std::make_shared<toml_combined_log_sink_config_parser>());
    }

    //! \copydoc num_collect::logging::config::log_config_parser_base::parse_from_file
    void parse_from_file(std::string_view filepath) override {
        if (!std::filesystem::exists(std::filesystem::path(filepath)) ||
            !std::filesystem::is_regular_file(
                std::filesystem::path(filepath))) {
            throw invalid_argument(fmt::format(
                "Invalid filepath to load configurations {}.", filepath));
        }
        try {
            const auto parse_result = ::toml::parse_file(filepath);
            parse_from_table(parse_result);
        } catch (const std::exception& e) {
            throw invalid_argument(
                fmt::format("Failed to load {}: {}", filepath, e.what()));
        }
    }

    //! \copydoc num_collect::logging::config::log_config_parser_base::parse_from_text
    void parse_from_text(std::string_view text) override {
        try {
            const auto parse_result = ::toml::parse(text);
            parse_from_table(parse_result);
        } catch (const std::exception& e) {
            throw invalid_argument(
                fmt::format("Failed to load from a text: {}\n"
                            "Input:\n"
                            "{}",
                    e.what(), text));
        }
    }

    /*!
     * \brief Parse configuration from a TOML table.
     *
     * \param[in] table Table.
     */
    void parse_from_table(const ::toml::table& table) {
        log_sink_factory_table sinks;

        const auto log_sink_configs_node =
            table.at_path("num_collect.logging.sinks");
        if (log_sink_configs_node) {
            const auto* log_sink_configs_array =
                log_sink_configs_node.as_array();
            if (log_sink_configs_array == nullptr) {
                throw invalid_argument(
                    "Configuration num_collect.logging.sinks must be an "
                    "array.");
            }
            parse_log_sinks(*log_sink_configs_array, sinks);
        }

        const auto log_tag_configs_node =
            table.at_path("num_collect.logging.tag_configs");
        if (log_tag_configs_node) {
            const auto* log_tag_configs_array = log_tag_configs_node.as_array();
            if (log_tag_configs_array == nullptr) {
                throw invalid_argument(
                    "Configuration num_collect.logging.tag_configs must be an "
                    "array.");
            }
            parse_and_apply_log_tag_configs(*log_tag_configs_array, sinks);
        }
    }

    /*!
     * \brief Append a parser of configurations of log sinks.
     *
     * \param[in] name Name.
     * \param[in] parser Parser.
     */
    void append_log_sink_config_parser(const std::string& name,
        const std::shared_ptr<toml_log_sink_config_parser_base>& parser) {
        if (!log_sink_config_parsers_.try_emplace(name, parser).second) {
            throw invalid_argument(fmt::format(
                "Duplicate name of parsers of configurations of log sinks {}.",
                name));
        }
    }

    /*!
     * \brief Destructor.
     */
    ~toml_log_config_parser() noexcept override = default;

    toml_log_config_parser(const toml_log_config_parser&) = delete;
    toml_log_config_parser(toml_log_config_parser&&) = delete;
    auto operator=(const toml_log_config_parser&)
        -> toml_log_config_parser& = delete;
    auto operator=(toml_log_config_parser&&)
        -> toml_log_config_parser& = delete;

private:
    /*!
     * \brief Parse configurations of log sinks.
     *
     * \param[in] array TOML array.
     * \param[out] sinks Log sinks.
     */
    void parse_log_sinks(
        const ::toml::array& array, log_sink_factory_table& sinks) {
        for (const auto& elem_node : array) {
            const auto* elem_table = elem_node.as_table();
            if (elem_table == nullptr) {
                throw invalid_argument(
                    "Each element in configuration num_collect.logging.sinks "
                    "must be a table.");
            }

            const auto name = require_value<std::string>(*elem_table, "name",
                "name in num_collect.logging.sinks element", "a string");
            const auto type = require_value<std::string>(*elem_table, "type",
                "type in num_collect.logging.sinks element", "a string");

            const auto parser_iter = log_sink_config_parsers_.find(type);
            if (parser_iter == log_sink_config_parsers_.end()) {
                throw invalid_argument(
                    fmt::format("Invalid type of log sink {}.", type));
            }
            const auto sink_factory = parser_iter->second->parse(*elem_table);

            sinks.append(name, sink_factory);
        }
    }

    /*!
     * \brief Parse and apply configurations of log tags.
     *
     * \param[in] array TOML array.
     * \param[in] sinks Log sinks.
     */
    static void parse_and_apply_log_tag_configs(
        const ::toml::array& array, log_sink_factory_table& sinks) {
        for (const auto& elem_node : array) {
            const auto* elem_table = elem_node.as_table();
            if (elem_table == nullptr) {
                throw invalid_argument(
                    "Each element in configuration "
                    "num_collect.logging.tag_configs "
                    "must be a table.");
            }

            const auto tag_string = require_value<std::string>(*elem_table,
                "tag", "tag in num_collect.logging.tag_configs element",
                "a string");
            const auto tag = log_tag(tag_string);

            auto config = get_config_of(tag);

            impl::parse_log_tag_config_to(config, *elem_table, sinks);

            set_config_of(tag, config);
        }
    }

    //! Parses of configurations of log sinks.
    std::unordered_map<std::string,
        std::shared_ptr<toml_log_sink_config_parser_base>>
        log_sink_config_parsers_{};
};

}  // namespace num_collect::logging::config::toml
