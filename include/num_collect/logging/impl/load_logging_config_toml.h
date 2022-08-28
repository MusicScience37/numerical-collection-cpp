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
 * \brief Definition of load_logging_config_toml function.
 */
#pragma once

#include <cstdio>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>
#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/colored_console_log_sink.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_sink_base.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/simple_log_sink.h"

namespace num_collect::logging::impl::toml_config {

//! Name of the default log sink.
constexpr auto default_log_sink_name = std::string_view("default");

/*!
 * \brief Get a value or throw exceptions.
 *
 * \tparam T Type of the value.
 * \param[in] table Table in toml++ library.
 * \param[in] path Path.
 * \param[in] config_name Configuration name for exceptions.
 * \param[in] type_name Type name for exceptions.
 * \return Value.
 */
template <typename T>
inline auto require_value(const toml::table& table, std::string_view path,
    std::string_view config_name, std::string_view type_name) -> T {
    const auto child_node = table.at_path(path);
    if (!child_node) {
        throw invalid_argument(
            fmt::format("Configuration {} is required.", config_name));
    }
    const std::optional<T> value = child_node.value<T>();
    if (!value) {
        throw invalid_argument(fmt::format(
            "Configuration {} must be {}.", config_name, type_name));
    }
    return *value;
}

/*!
 * \brief Get a log level value or throw exception.
 *
 * \param[in] table Table in toml++ library.
 * \param[in] path Path.
 * \param[in] config_name Configuration name for exceptions.
 * \return Value.
 */
inline auto require_log_level(const toml::table& table, std::string_view path,
    std::string_view config_name) -> log_level {
    const auto str =
        require_value<std::string>(table, path, config_name, "a string");
    if (str == "trace") {
        return log_level::trace;
    }
    if (str == "debug") {
        return log_level::debug;
    }
    if (str == "iteration") {
        return log_level::iteration;
    }
    if (str == "summary") {
        return log_level::summary;
    }
    if (str == "info") {
        return log_level::info;
    }
    if (str == "warning") {
        return log_level::warning;
    }
    if (str == "error") {
        return log_level::error;
    }
    if (str == "critical") {
        return log_level::critical;
    }
    if (str == "off") {
        return log_level::off;
    }
    throw invalid_argument(fmt::format("Invalid log level {}.", str));
}

/*!
 * \brief Parse configurations of a log sink.
 *
 * \param[in] table Table in toml++ library.
 * \return Name and log sink.
 */
inline auto parse_log_sink_config(const toml::table& table)
    -> std::pair<std::string, std::shared_ptr<log_sink_base>> {
    const auto name = require_value<std::string>(
        table, "name", "name in num_collect.logging.sinks element", "a string");
    if (name == default_log_sink_name) {
        throw invalid_argument(fmt::format(
            "Log sink name {} is reserved.", default_log_sink_name));
    }

    const auto type = require_value<std::string>(
        table, "type", "type in num_collect.logging.sinks element", "a string");

    if (type == "colored_console") {
        std::FILE* file = stdout;
        return {name, std::make_shared<colored_console_log_sink>(file)};
    }

    if (type == "single_file") {
        const auto filepath = require_value<std::string>(table, "filepath",
            "filepath in num_collect.logging.sinks element", "a string");
        return {name, std::make_shared<simple_log_sink>(filepath)};
    }

    throw invalid_argument(fmt::format("Invalid log sink type {}.", type));
}

/*!
 * \brief Parse configurations of logging for a log tag.
 *
 * \param[in] table Table in toml++ library.
 * \param[in] sinks Log sinks.
 * \return Log tag and its configuration.
 */
inline auto parse_log_tag_config(const toml::table& table,
    const std::unordered_map<std::string, std::shared_ptr<log_sink_base>>&
        sinks) -> std::pair<log_tag, log_tag_config> {
    const auto tag_string = require_value<std::string>(table, "tag",
        "tag in num_collect.logging.tag_configs element", "a string");
    const auto tag = log_tag(tag_string);

    log_tag_config config;

    if (table.contains("sink")) {
        const auto sink_name = require_value<std::string>(table, "sink",
            "sink in num_collect.logging.tag_configs element", "a string");
        const auto iter = sinks.find(sink_name);
        if (iter == sinks.end()) {
            throw invalid_argument(
                fmt::format("Log sink {} not found", sink_name));
        }
        config.sink(iter->second);
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

    return {tag, config};
}

/*!
 * \brief Parse configurations of log sinks.
 *
 * \param[in] array Array in toml++ library.
 * \return Log sinks.
 */
inline auto parse_log_sinks(const toml::array& array)
    -> std::unordered_map<std::string, std::shared_ptr<log_sink_base>> {
    std::unordered_map<std::string, std::shared_ptr<log_sink_base>> sinks;
    for (const auto& elem_node : array) {
        const auto* elem_table = elem_node.as_table();
        if (elem_table == nullptr) {
            throw invalid_argument(
                "Each element in configuration num_collect.logging.sinks "
                "must be a table.");
        }
        sinks.insert(parse_log_sink_config(*elem_table));
    }
    return sinks;
}

/*!
 * \brief Parse and apply configurations of logging for log tags.
 *
 * \param[in] array Array in toml++ library.
 * \param[in] sinks Log sinks.
 */
inline void load_log_tag_configs(const toml::array& array,
    const std::unordered_map<std::string, std::shared_ptr<log_sink_base>>&
        sinks) {
    for (const auto& elem_node : array) {
        const auto* elem_table = elem_node.as_table();
        if (elem_table == nullptr) {
            throw invalid_argument(
                "Each element in configuration num_collect.logging.tag_configs "
                "must be a table.");
        }
        const auto [tag, config] = parse_log_tag_config(*elem_table, sinks);
        log_config::instance().set_config_of(tag, config);
    }
}

/*!
 * \brief Parse and apply configurations of logging from a table in toml++
 * library.
 *
 * \param[in] table Base table.
 */
inline void load_logging_config_toml(const toml::table& table) {
    std::unordered_map<std::string, std::shared_ptr<log_sink_base>> sinks;

    const auto log_sink_configs_node =
        table.at_path("num_collect.logging.sinks");
    if (log_sink_configs_node) {
        const auto* log_sink_configs_array = log_sink_configs_node.as_array();
        if (log_sink_configs_array == nullptr) {
            throw invalid_argument(
                "Configuration num_collect.logging.sinks must be an "
                "array.");
        }
        sinks = parse_log_sinks(*log_sink_configs_array);
    }

    sinks.try_emplace(
        std::string(default_log_sink_name), get_default_log_sink());

    const auto log_tag_configs_node =
        table.at_path("num_collect.logging.tag_configs");
    if (log_tag_configs_node) {
        const auto* log_tag_configs_array = log_tag_configs_node.as_array();
        if (log_tag_configs_array == nullptr) {
            throw invalid_argument(
                "Configuration num_collect.logging.tag_configs must be an "
                "array.");
        }
        load_log_tag_configs(*log_tag_configs_array, sinks);
    }
}

}  // namespace num_collect::logging::impl::toml_config
