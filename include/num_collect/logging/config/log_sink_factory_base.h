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
 * \brief Definition of log_sink_factory_base class.
 */
#pragma once

#include "num_collect/logging/sinks/log_sink.h"

namespace num_collect::logging::config {

class log_sink_factory_table;

/*!
 * \brief Interface of factories of log sinks.
 */
class log_sink_factory_base {
public:
    /*!
     * \brief Create log sink.
     *
     * \param[in] sinks Other log sinks.
     * \return Created log sink.
     */
    [[nodiscard]] virtual auto create(log_sink_factory_table& sinks)
        -> sinks::log_sink = 0;

    log_sink_factory_base(const log_sink_factory_base&) = delete;
    log_sink_factory_base(log_sink_factory_base&&) = delete;
    auto operator=(const log_sink_factory_base&)
        -> log_sink_factory_base& = delete;
    auto operator=(log_sink_factory_base&&) -> log_sink_factory_base& = delete;

    /*!
     * \brief Destructor.
     */
    virtual ~log_sink_factory_base() noexcept = default;

protected:
    /*!
     * \brief Constructor.
     */
    log_sink_factory_base() noexcept = default;
};

}  // namespace num_collect::logging::config
