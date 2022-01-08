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
 * \brief Definition of log_sink_base class.
 */
#pragma once

#include <chrono>
#include <string_view>

#include <fmt/format.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging {

/*!
 * \brief Interface of log sinks.
 *
 */
class log_sink_base {
public:
    /*!
     * \brief Write a log.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     *
     * \note Implementations of this function must be thread-safe.
     */
    virtual void write(std::chrono::system_clock::time_point time,
        std::string_view tag, log_level level, source_info_view source,
        std::string_view body) noexcept = 0;

    log_sink_base(const log_sink_base&) = delete;
    log_sink_base(log_sink_base&&) = delete;
    auto operator=(const log_sink_base&) -> log_sink_base& = delete;
    auto operator=(log_sink_base&&) -> log_sink_base& = delete;

    /*!
     * \brief Destruct.
     */
    virtual ~log_sink_base() noexcept = default;

protected:
    /*!
     * \brief Construct.
     */
    log_sink_base() noexcept = default;
};

}  // namespace num_collect::logging
