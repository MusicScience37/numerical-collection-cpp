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
 * \brief Definition of load_logging_config function.
 */
#pragma once

#include <exception>

#include <fmt/format.h>
#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/impl/load_logging_config_toml.h"

namespace num_collect::logging {

/*!
 * \brief Parse and apply configurations of logging from a file.
 *
 * \param[in] filepath File path.
 */
inline void load_logging_config(const std::string& filepath) {
    try {
        impl::toml_config::load_logging_config_toml(toml::parse_file(filepath));
    } catch (const std::exception& e) {
        throw invalid_argument(
            fmt::format("Failed to load {}: {}", filepath, e.what()));
    }
}

}  // namespace num_collect::logging
