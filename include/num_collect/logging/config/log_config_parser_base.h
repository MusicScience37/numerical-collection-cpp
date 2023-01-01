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
 * \brief Definition of log_config_parser_base class.
 */
#pragma once

#include <string_view>

namespace num_collect::logging::config {

/*!
 * \brief Interface of parsers of logging configurations.
 */
class log_config_parser_base {
public:
    /*!
     * \brief Parse configuration from a file.
     *
     * \param[in] filepath Filepath.
     */
    virtual void parse_from_file(std::string_view filepath) = 0;

    /*!
     * \brief Parse configuration from a file.
     *
     * \param[in] text Text.
     */
    virtual void parse_from_text(std::string_view text) = 0;

    log_config_parser_base(const log_config_parser_base&) = delete;
    log_config_parser_base(log_config_parser_base&&) = delete;
    auto operator=(const log_config_parser_base&)
        -> log_config_parser_base& = delete;
    auto operator=(log_config_parser_base&&)
        -> log_config_parser_base& = delete;

    /*!
     * \brief Destructor.
     */
    virtual ~log_config_parser_base() noexcept = default;

protected:
    /*!
     * \brief Constructor.
     */
    log_config_parser_base() noexcept = default;
};

}  // namespace num_collect::logging::config
