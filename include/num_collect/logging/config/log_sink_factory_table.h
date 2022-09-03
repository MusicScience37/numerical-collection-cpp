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
 * \brief Definition of log_sink_factory_table class.
 */
#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>

#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/sinks/log_sink_base.h"

namespace num_collect::logging::config {

/*!
 * \brief Class of tables of log sinks.
 */
class log_sink_factory_table {
public:
    //! Constructor.
    log_sink_factory_table() = default;

    /*!
     * \brief Append a factory of a log sink.
     *
     * \param[in] name Name of the log sink.
     * \param[in] sink_factory Factory of the log sink.
     */
    void append(
        std::string name, std::shared_ptr<log_sink_factory_base> sink_factory) {
        caches_.try_emplace(std::move(name), std::move(sink_factory));
    }

    /*!
     * \brief Get a log sink creating it if needed.
     *
     * \param[in] name Name of the log sink.
     * \return Log sink.
     */
    [[nodiscard]] auto get(const std::string& name)
        -> std::shared_ptr<sinks::log_sink_base> {
        const auto iter = caches_.find(name);
        if (iter == caches_.end()) {
            throw std::invalid_argument(
                fmt::format("Log sink {} not found.", name));
        }
        return iter->second.get(*this);
    }

private:
    //! Class of caches of log sinks.
    class cached_log_sink {
    public:
        /*!
         * \brief Constructor.
         *
         * \param[in] factory Factory.
         */
        explicit cached_log_sink(std::shared_ptr<log_sink_factory_base> factory)
            : factory_(std::move(factory)) {}

        /*!
         * \brief Get the log sink creating it if needed.
         *
         * \param[in] sinks Other log sinks.
         * \return Sink.
         */
        [[nodiscard]] auto get(log_sink_factory_table& sinks)
            -> std::shared_ptr<sinks::log_sink_base> {
            if (!sink_) {
                sink_ = factory_->create(sinks);
            }
            return sink_;
        }

    private:
        //! Factory of the log sink.
        std::shared_ptr<log_sink_factory_base> factory_;

        //! Log sink.
        std::shared_ptr<sinks::log_sink_base> sink_{};
    };

    //! Caches.
    std::unordered_map<std::string, cached_log_sink> caches_{};
};

}  // namespace num_collect::logging::config
