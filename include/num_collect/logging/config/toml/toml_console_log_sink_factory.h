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
 * \brief Definition of toml_console_log_sink_factory class.
 */
#pragma once

#include <toml++/toml.h>

#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/config/log_sink_factory_table.h"
#include "num_collect/logging/config/toml/toml_helper.h"
#include "num_collect/logging/sinks/console_log_sink.h"
#include "num_collect/logging/sinks/log_sink.h"

namespace num_collect::logging::config::toml {

/*!
 * \brief Class to create log sinks to write to consoles using TOML
 * configurations.
 *
 * \thread_safety Not thread-safe.
 */
class toml_console_log_sink_factory final : public log_sink_factory_base {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] table Table.
     */
    explicit toml_console_log_sink_factory(const ::toml::table& table) {
        read_optional_value_to(use_color_, table, "use_color",
            "use_color in num_collect.logging.sinks element with type "
            "\"console\"",
            "a boolean");
    }

    //! \copydoc num_collect::logging::config::log_sink_factory_base::create
    [[nodiscard]] auto create(
        log_sink_factory_table& sinks) -> sinks::log_sink override {
        (void)sinks;
        if (use_color_) {
            return sinks::create_colored_console_sink();
        }
        return sinks::create_non_colored_console_sink();
    }

    /*!
     * \brief Destructor.
     */
    ~toml_console_log_sink_factory() noexcept override = default;

    toml_console_log_sink_factory(
        const toml_console_log_sink_factory&) = delete;
    toml_console_log_sink_factory(toml_console_log_sink_factory&&) = delete;
    auto operator=(const toml_console_log_sink_factory&)
        -> toml_console_log_sink_factory& = delete;
    auto operator=(toml_console_log_sink_factory&&)
        -> toml_console_log_sink_factory& = delete;

private:
    //! Whether to use color.
    bool use_color_{true};
};

}  // namespace num_collect::logging::config::toml
