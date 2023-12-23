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
 * \brief Definition of default_log_sink_factory class.
 */
#pragma once

#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/sinks/default_log_sink.h"
#include "num_collect/logging/sinks/log_sink.h"

namespace num_collect::logging::config {

/*!
 * \brief Class to create default log sink.
 *
 * \thread_safety Thread-safe for all operations.
 */
class default_log_sink_factory final : public log_sink_factory_base {
public:
    /*!
     * \brief Constructor.
     */
    default_log_sink_factory() = default;

    //! \copydoc num_collect::logging::config::log_sink_factory_base::create
    [[nodiscard]] auto create(log_sink_factory_table& sinks)
        -> sinks::log_sink override {
        (void)sinks;

        return sinks::get_default_log_sink();
    }

    /*!
     * \brief Destructor.
     */
    ~default_log_sink_factory() noexcept override = default;

    default_log_sink_factory(const default_log_sink_factory&) = delete;
    default_log_sink_factory(default_log_sink_factory&&) = delete;
    auto operator=(const default_log_sink_factory&)
        -> default_log_sink_factory& = delete;
    auto operator=(default_log_sink_factory&&)
        -> default_log_sink_factory& = delete;
};

}  // namespace num_collect::logging::config
