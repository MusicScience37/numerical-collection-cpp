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
 * \brief Definition of toml_parser class.
 */
#pragma once

#include <string_view>

#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"

/*!
 * \brief Class to parse TOML files.
 */
class toml_parser {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] file_path Path to the TOML file.
     */
    explicit toml_parser(std::string_view file_path)
        : config_(toml::parse_file(file_path)) {}

    /*!
     * \brief Get a value.
     *
     * \tparam T Type of the value.
     * \param[in] path Path to the value in the TOML file.
     * \return Value.
     */
    template <typename T>
    [[nodiscard]] auto get(std::string_view path) const -> T {
        const auto value = config_.at_path(path).value<T>();
        if (!value) {
            NUM_COLLECT_LOG_AND_THROW(num_collect::invalid_argument,
                "Failed to get the value of configuration {}.", path);
        }
        return value.value();
    }

private:
    toml::table config_;
};
