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
 * \brief Definition of toml_async_log_sink_factory class.
 */
#pragma once

#include <memory>
#include <string>

#include <toml++/toml.h>

#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/config/log_sink_factory_table.h"
#include "num_collect/logging/config/toml/toml_helper.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/logging/sinks/log_sinks.h"

namespace num_collect::logging::config::toml {

/*!
 * \brief Class to create log sinks to write logs asynchronously.
 *
 * \thread_safety Not thread-safe.
 */
class toml_async_log_sink_factory final : public log_sink_factory_base {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] table Table.
     */
    explicit toml_async_log_sink_factory(const ::toml::table& table) {
        inner_sink_name_ = require_value<std::string>(table, "inner_sink_name",
            "inner_sink_name in num_collect.logging.sinks element with type "
            "\"async\"",
            "a string");
    }

    //! \copydoc num_collect::logging::config::log_sink_factory_base::create
    [[nodiscard]] auto create(log_sink_factory_table& sinks)
        -> std::shared_ptr<sinks::log_sink_base> override {
        return sinks::create_async_log_sink(sinks.get(inner_sink_name_));
    }

    /*!
     * \brief Destructor.
     */
    ~toml_async_log_sink_factory() noexcept override = default;

    toml_async_log_sink_factory(const toml_async_log_sink_factory&) = delete;
    toml_async_log_sink_factory(toml_async_log_sink_factory&&) = delete;
    auto operator=(const toml_async_log_sink_factory&)
        -> toml_async_log_sink_factory& = delete;
    auto operator=(toml_async_log_sink_factory&&)
        -> toml_async_log_sink_factory& = delete;

private:
    //! Log sink used to actual logging in asynchronous processing.
    std::string inner_sink_name_;
};

}  // namespace num_collect::logging::config::toml
