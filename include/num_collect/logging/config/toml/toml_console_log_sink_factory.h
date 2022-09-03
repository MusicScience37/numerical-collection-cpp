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

#include <memory>
#include <utility>

#include <toml++/toml.h>

#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/config/toml/toml_helper.h"
#include "num_collect/logging/formatters/colored_compact_log_formatter.h"
#include "num_collect/logging/formatters/compact_log_formatter.h"
#include "num_collect/logging/formatters/log_formatter_base.h"
#include "num_collect/logging/sinks/file_wrapper.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/logging/sinks/simple_log_sink.h"

namespace num_collect::logging::config::toml {

/*!
 * \brief Class to create log sinks to write to consoles using TOML
 * configuration.
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
    [[nodiscard]] auto create(log_sink_factory_table& sinks)
        -> std::shared_ptr<sinks::log_sink_base> override {
        (void)sinks;

        sinks::file_wrapper file{};
        file.set_stdout();
        return std::make_shared<sinks::simple_log_sink>(
            std::move(file), create_formatter());
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
    /*!
     * \brief Create a formatter.
     *
     * \return Formatter.
     */
    [[nodiscard]] auto create_formatter() const
        -> std::shared_ptr<formatters::log_formatter_base> {
        if (use_color_) {
            return std::make_shared<
                formatters::colored_compact_log_formatter>();
        }
        return std::make_shared<formatters::compact_log_formatter>();
    }

    //! Whether to use color.
    bool use_color_{true};
};

}  // namespace num_collect::logging::config::toml
