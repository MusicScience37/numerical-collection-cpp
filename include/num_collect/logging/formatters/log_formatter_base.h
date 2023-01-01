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
 * \brief Definition of log_formatter_base class.
 */
#pragma once

#include <chrono>
#include <string_view>

#include <fmt/format.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::formatters {

/*!
 * \brief Base class of classes to format logs.
 */
class log_formatter_base {
public:
    /*!
     * \brief Format a log.
     *
     * \param[out] buffer Buffer to write the formatted log.
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     */
    virtual void format(fmt::memory_buffer& buffer,
        std::chrono::system_clock::time_point time, std::string_view tag,
        log_level level, util::source_info_view source,
        std::string_view body) = 0;

    //! Destructor.
    virtual ~log_formatter_base() = default;

    log_formatter_base(const log_formatter_base&) = delete;
    log_formatter_base(log_formatter_base&&) = delete;
    auto operator=(const log_formatter_base&) = delete;
    auto operator=(log_formatter_base&&) = delete;

protected:
    //! Constructor.
    log_formatter_base() = default;
};

}  // namespace num_collect::logging::formatters
