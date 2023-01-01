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
 * \brief Definition of helper functions for parsing TOML files.
 */
#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <toml++/toml.h>

#include "num_collect/base/exception.h"

namespace num_collect::logging::config::toml {

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
inline auto require_value(const ::toml::table& table, std::string_view path,
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
 * \brief Read a value to a variable.
 *
 * \tparam T Type of the value.
 * \param[out] output Output variable.
 * \param[in] table Table in toml++ library.
 * \param[in] path Path.
 * \param[in] config_name Configuration name for exceptions.
 * \param[in] type_name Type name for exceptions.
 */
template <typename T>
inline void read_optional_value_to(T& output, const ::toml::table& table,
    std::string_view path, std::string_view config_name,
    std::string_view type_name) {
    const auto child_node = table.at_path(path);
    if (!child_node) {
        return;
    }
    const std::optional<T> value = child_node.value<T>();
    if (!value) {
        throw invalid_argument(fmt::format(
            "Configuration {} must be {}.", config_name, type_name));
    }
    output = *value;
}

}  // namespace num_collect::logging::config::toml
