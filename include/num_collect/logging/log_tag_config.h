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
 * \brief Definition of log_tag_config class.
 */
#pragma once

#include <cstdio>
#include <memory>
#include <utility>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/colored_console_log_sink.h"
#include "num_collect/logging/log_sink_base.h"

namespace num_collect::logging {

namespace impl {

/*!
 * \brief Get the default log sink.
 *
 * \return Log sink.
 */
[[nodiscard]] inline auto get_default_log_sink()
    -> std::shared_ptr<log_sink_base> {
    static auto sink = std::make_shared<colored_console_log_sink>(stdout);
    return sink;
}

}  // namespace impl

/*!
 * \brief Class to hold configurations for log tags.
 */
class log_tag_config {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] sink Log sink.
     */
    log_tag_config() = default;

    /*!
     * \brief Get the log sink.
     *
     * \return Log sink.
     */
    [[nodiscard]] auto sink() const noexcept -> std::shared_ptr<log_sink_base> {
        return sink_;
    }

    /*!
     * \brief Set the log sink.
     *
     * \param[in] val Log sink.
     * \return This.
     */
    auto sink(std::shared_ptr<log_sink_base> val) -> log_tag_config& {
        if (!val) {
            throw assertion_failure("Null sink.");
        }
        sink_ = std::move(val);
        return *this;
    }

    /*!
     * \brief Get whether to write trace logs.
     *
     * \return value.
     */
    [[nodiscard]] auto write_traces() const noexcept -> bool {
        return write_traces_;
    }

    /*!
     * \brief Set whether to write trace logs.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto write_traces(bool val) -> log_tag_config& {
        write_traces_ = val;
        return *this;
    }

    /*!
     * \brief Get whether to write iteration logs.
     *
     * \return value.
     */
    [[nodiscard]] auto write_iterations() const noexcept -> bool {
        return write_iterations_;
    }

    /*!
     * \brief Set whether to write iteration logs.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto write_iterations(bool val) -> log_tag_config& {
        write_iterations_ = val;
        return *this;
    }

    /*!
     * \brief Get whether to write summary logs.
     *
     * \return value.
     */
    [[nodiscard]] auto write_summary() const noexcept -> bool {
        return write_summary_;
    }

    /*!
     * \brief Set whether to write summary logs.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto write_summary(bool val) -> log_tag_config& {
        write_summary_ = val;
        return *this;
    }

    /*!
     * \brief Get the period to write iteration logs.
     *
     * \return value.
     */
    [[nodiscard]] auto iteration_output_period() const noexcept -> index_type {
        return iteration_output_period_;
    }

    /*!
     * \brief Set the period to write iteration logs.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto iteration_output_period(index_type val) -> log_tag_config& {
        if (val <= 0) {
            throw assertion_failure(
                "iteration_output_period must be a positive integer.");
        }
        iteration_output_period_ = val;
        return *this;
    }

    /*!
     * \brief Get the period to write labels of iteration logs.
     *
     * \return value.
     */
    [[nodiscard]] auto iteration_label_period() const noexcept -> index_type {
        return iteration_label_period_;
    }

    /*!
     * \brief Set the period to write labels of iteration logs.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto iteration_label_period(index_type val) -> log_tag_config& {
        if (val <= 0) {
            throw assertion_failure(
                "iteration_label_period must be a positive integer.");
        }
        iteration_label_period_ = val;
        return *this;
    }

private:
    //! Log sink.
    std::shared_ptr<log_sink_base> sink_{impl::get_default_log_sink()};

    //! Whether to write trace logs.
    bool write_traces_{false};

    //! Whether to write iteration logs.
    bool write_iterations_{false};

    //! Whether to write summary logs.
    bool write_summary_{false};

    //! Default period to write iteration logs.
    static constexpr index_type default_iteration_output_period = 10;

    //! Period to write iteration logs.
    index_type iteration_output_period_{default_iteration_output_period};

    //! Default period to write labels of iteration logs.
    static constexpr index_type default_iteration_label_period = 20;

    //! Period to write labels of iteration logs.
    index_type iteration_label_period_{default_iteration_label_period};
};

}  // namespace num_collect::logging
