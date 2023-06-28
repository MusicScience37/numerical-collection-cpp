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
 * \brief Definition of toml_combined_log_sink_factory class.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/config/log_sink_factory_table.h"
#include "num_collect/logging/config/parse_output_log_level_str.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/combined_log_sink.h"
#include "num_collect/logging/sinks/log_sink_base.h"

namespace num_collect::logging::config::toml {

/*!
 * \brief Class to create log sinks to write logs to multiple log sinks.
 *
 * \thread_safety Not thread-safe.
 */
class toml_combined_log_sink_factory final : public log_sink_factory_base {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] table Table.
     */
    explicit toml_combined_log_sink_factory(const ::toml::table& table) {
        const auto inner_sink_names_node = table.at_path("inner_sink_names");
        if (!inner_sink_names_node) {
            throw invalid_argument(
                "Configuration inner_sink_names in num_collect.logging.sinks "
                "element with type \"combined\" is required.");
        }
        const auto* inner_sink_names_array = inner_sink_names_node.as_array();
        if (inner_sink_names_array == nullptr) {
            throw invalid_argument(
                "Configuration inner_sink_names in num_collect.logging.sinks "
                "element with type \"combined\" must be an array of strings.");
        }
        inner_sink_names_.reserve(inner_sink_names_array->size());
        for (const auto& inner_sink_name_node : *inner_sink_names_array) {
            const auto inner_sink_name =
                inner_sink_name_node.value<std::string>();
            if (!inner_sink_name) {
                throw invalid_argument(
                    "Configuration inner_sink_names in "
                    "num_collect.logging.sinks element with type \"combined\" "
                    "must be an array of strings.");
            }
            inner_sink_names_.push_back(*inner_sink_name);
        }

        const auto output_log_level_strs_node =
            table.at_path("output_log_levels");
        if (!output_log_level_strs_node) {
            throw invalid_argument(
                "Configuration output_log_levels in num_collect.logging.sinks "
                "element with type \"combined\" is required.");
        }
        const auto* output_log_level_strs_array =
            output_log_level_strs_node.as_array();
        if (output_log_level_strs_array == nullptr) {
            throw invalid_argument(
                "Configuration output_log_levels in num_collect.logging.sinks "
                "element with type \"combined\" must be an array of strings.");
        }
        output_log_levels_.reserve(output_log_level_strs_array->size());
        for (const auto& output_log_level_str_node :
            *output_log_level_strs_array) {
            const auto output_log_level_str =
                output_log_level_str_node.value<std::string>();
            if (!output_log_level_str) {
                throw invalid_argument(
                    "Configuration output_log_levels in "
                    "num_collect.logging.sinks element with type \"combined\" "
                    "must be an array of strings.");
            }
            output_log_levels_.push_back(
                parse_output_log_level_str(*output_log_level_str));
        }

        if (inner_sink_names_.size() != output_log_levels_.size()) {
            throw invalid_argument(
                "inner_sink_names and output_log_levels in "
                "num_collect.logging.sinks element with type \"combined\" must "
                "have the same number of elements.");
        }
    }

    //! \copydoc num_collect::logging::config::log_sink_factory_base::create
    [[nodiscard]] auto create(log_sink_factory_table& sinks)
        -> std::shared_ptr<sinks::log_sink_base> override {
        std::vector<std::pair<std::shared_ptr<sinks::log_sink_base>, log_level>>
            inner_sinks;
        inner_sinks.reserve(inner_sink_names_.size());
        for (std::size_t i = 0; i < inner_sink_names_.size(); ++i) {
            inner_sinks.emplace_back(
                sinks.get(inner_sink_names_[i]), output_log_levels_[i]);
        }
        return sinks::create_combined_log_sink(std::move(inner_sinks));
    }

    /*!
     * \brief Destructor.
     */
    ~toml_combined_log_sink_factory() noexcept override = default;

    toml_combined_log_sink_factory(
        const toml_combined_log_sink_factory&) = delete;
    toml_combined_log_sink_factory(toml_combined_log_sink_factory&&) = delete;
    auto operator=(const toml_combined_log_sink_factory&)
        -> toml_combined_log_sink_factory& = delete;
    auto operator=(toml_combined_log_sink_factory&&)
        -> toml_combined_log_sink_factory& = delete;

private:
    //! Names of log sinks.
    std::vector<std::string> inner_sink_names_;

    //! Output log levels of log sinks.
    std::vector<log_level> output_log_levels_;
};

}  // namespace num_collect::logging::config::toml
