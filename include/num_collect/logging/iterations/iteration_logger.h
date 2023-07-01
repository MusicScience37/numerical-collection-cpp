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
 * \brief Definition of iteration_logger class.
 */
#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <fmt/format.h>

#include "num_collect/logging/concepts/formattable_iteration_parameter_value.h"  // IWYU pragma: keep
#include "num_collect/logging/concepts/getter_of.h"  // IWYU pragma: keep
#include "num_collect/logging/concepts/iteration_parameter_value.h"  // IWYU pragma: keep
#include "num_collect/logging/concepts/member_getter_of.h"  // IWYU pragma: keep
#include "num_collect/logging/iterations/function_iteration_parameter_value.h"
#include "num_collect/logging/iterations/iteration_parameter.h"
#include "num_collect/logging/iterations/member_function_iteration_parameter_value.h"
#include "num_collect/logging/iterations/member_variable_iteration_parameter_value.h"
#include "num_collect/logging/iterations/variable_iteration_parameter_value.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/util/iteration_period_checker.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::iterations {

/*!
 * \brief Class to write logs of iterations.
 *
 * \tparam Algorithm Type of the algorithm. If the pointer of an algorithm is
 * not used, this template parameter can be any type as the default type.
 */
template <typename Algorithm = std::monostate>
class iteration_logger {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     */
    explicit iteration_logger(logger& logger)
        : tag_(logger.tag()),
          sink_(logger.config().sink()),
          iteration_output_period_checker_(
              logger.config().iteration_output_period()),
          iteration_label_period_checker_(
              logger.config().iteration_label_period()) {
        start(logger);
    }

    /*!
     * \brief Start iterations.
     *
     * \param[in] logger Logger.
     */
    void start(logger& logger) {
        write_iterations_ = logger.should_log(log_level::iteration);
        write_summaries_ = logger.should_log(log_level::summary);
        iteration_output_period_checker_.reset();
        iteration_label_period_checker_.reset();
    }

    /*!
     * \brief Append a parameter.
     *
     * \param[in] parameter Parameter.
     */
    void append(
        std::shared_ptr<iteration_parameter_base<Algorithm>> parameter) {
        parameters_.push_back(std::move(parameter));
    }

    /*!
     * \brief Append a parameter.
     *
     * \tparam Value Type of values.
     * \tparam ParameterValue Class of parameters values in iterations
     * specified by variables.
     * \param[in] label Label.
     * \param[in] value Parameter value.
     * \return Parameter.
     */
    template <concepts::formattable_iteration_parameter_value Value,
        concepts::iteration_parameter_value<Algorithm, Value> ParameterValue>
    auto append(std::string label, ParameterValue value)
        -> iteration_parameter<Algorithm, Value, ParameterValue>* {
        auto parameter = std::make_shared<
            iteration_parameter<Algorithm, Value, ParameterValue>>(
            std::move(label), std::move(value));
        auto* raw_ptr = parameter.get();
        append(std::move(parameter));
        return raw_ptr;
    }

    /*!
     * \brief Append a parameter specified by variables.
     *
     * \tparam Value Type of values.
     * \param[in] label Label.
     * \param[in] value Parameter value.
     * \return Parameter.
     */
    template <concepts::formattable_iteration_parameter_value Value>
    auto append(std::string label, const Value& value)
        -> iteration_parameter<Algorithm, Value,
            variable_iteration_parameter_value<Algorithm, Value>>* {
        return append<Value>(std::move(label),
            variable_iteration_parameter_value<Algorithm, Value>(value));
    }

    /*!
     * \brief Append a parameter specified by functions.
     *
     * \tparam Value Type of values.
     * \tparam Function Type of the function.
     * \param[in] label Label.
     * \param[in] function Function to get values.
     * \return Parameter.
     */
    template <concepts::formattable_iteration_parameter_value Value,
        concepts::getter_of<Value> Function>
    auto append(std::string label, Function&& function)
        -> iteration_parameter<Algorithm, Value,
            function_iteration_parameter_value<Algorithm, Value,
                std::decay_t<Function>>>* {
        return append<Value>(std::move(label),
            function_iteration_parameter_value<Algorithm, Value,
                std::decay_t<Function>>(std::forward<Function>(function)));
    }

    /*!
     * \brief Append a parameter specified by member variables.
     *
     * \tparam Value Type of values.
     * \param[in] label Label.
     * \param[in] ptr Pointer to the member variable.
     * \return Parameter.
     */
    template <concepts::formattable_iteration_parameter_value Value>
    auto append(std::string label, const Value Algorithm::*ptr)
        -> iteration_parameter<Algorithm, Value,
            member_variable_iteration_parameter_value<Algorithm, Value>>* {
        return append<Value>(std::move(label),
            member_variable_iteration_parameter_value<Algorithm, Value>(ptr));
    }

    /*!
     * \brief Append a parameter specified by member functions.
     *
     * \tparam Value Type of values.
     * \tparam Function Type of the function.
     * \param[in] label Label.
     * \param[in] function Pointer to the member function.
     * \return Parameter.
     */
    template <concepts::formattable_iteration_parameter_value Value,
        concepts::member_getter_of<Value, Algorithm> Function>
    auto append(std::string label, Function&& function)
        -> iteration_parameter<Algorithm, Value,
            member_function_iteration_parameter_value<Algorithm, Value,
                std::decay_t<Function>>>* {
        return append<Value>(std::move(label),
            member_function_iteration_parameter_value<Algorithm, Value,
                std::decay_t<Function>>(std::forward<Function>(function)));
    }

    /*!
     * \brief Write an iteration to the logger.
     *
     * \param[in] source Information of the source code.
     *
     * \note This will write logs taking period configurations into account.
     */
    void write_iteration(
        util::source_info_view source = util::source_info_view()) {
        if (!should_write_iteration()) [[likely]] {
            return;
        }

        write_label_if_needed(source);

        buffer_.clear();
        format_values_to(buffer_);
        sink_->write(std::chrono::system_clock::now(), tag_.name(),
            log_level::iteration, source,
            std::string_view(buffer_.data(), buffer_.size()));

        ++iteration_output_period_checker_;
    }

    /*!
     * \brief Write an iteration to the logger.
     *
     * \param[in] algorithm Algorithm.
     * \param[in] source Information of the source code.
     *
     * \note This will write logs taking period configurations into account.
     */
    void write_iteration(Algorithm* algorithm,
        util::source_info_view source = util::source_info_view()) {
        if (!should_write_iteration()) [[likely]] {
            return;
        }

        write_label_if_needed(source);

        buffer_.clear();
        format_values_to(buffer_, algorithm);
        sink_->write(std::chrono::system_clock::now(), tag_.name(),
            log_level::iteration, source,
            std::string_view(buffer_.data(), buffer_.size()));

        ++iteration_output_period_checker_;
    }

    /*!
     * \brief Write a summary to a logger.
     *
     * \param[in] source Information of the source code.
     */
    void write_summary(
        util::source_info_view source = util::source_info_view()) {
        if (!write_summaries_) [[likely]] {
            return;
        }

        buffer_.clear();
        format_summary_to(buffer_);
        sink_->write(std::chrono::system_clock::now(), tag_.name(),
            log_level::summary, source,
            std::string_view(buffer_.data(), buffer_.size()));
    }

    /*!
     * \brief Write a summary to a logger.
     *
     * \param[in] algorithm Algorithm.
     * \param[in] source Information of the source code.
     */
    void write_summary(Algorithm* algorithm,
        util::source_info_view source = util::source_info_view()) {
        if (!write_summaries_) [[likely]] {
            return;
        }

        buffer_.clear();
        format_summary_to(buffer_, algorithm);
        sink_->write(std::chrono::system_clock::now(), tag_.name(),
            log_level::summary, source,
            std::string_view(buffer_.data(), buffer_.size()));
    }

private:
    /*!
     * \brief Determine whether to write an iteration to the logger.
     *
     * \return Whether to write an iteration to the logger.
     */
    [[nodiscard]] auto should_write_iteration() -> bool {
        if (!write_iterations_) [[likely]] {
            return false;
        }

        if (!iteration_output_period_checker_) [[likely]] {
            ++iteration_output_period_checker_;
            return false;
        }

        return true;
    }

    /*!
     * \brief Write labels if needed.
     *
     * \param[in] source Information of the source code.
     */
    void write_label_if_needed(util::source_info_view source) {
        if (iteration_label_period_checker_) {
            buffer_.clear();
            format_labels_to(buffer_);
            sink_->write(std::chrono::system_clock::now(), tag_.name(),
                log_level::iteration_label, source,
                std::string_view(buffer_.data(), buffer_.size()));
        }
        ++iteration_label_period_checker_;
    }

    /*!
     * \brief Format a line of labels.
     *
     * \param[out] buffer Buffer to format to.
     */
    void format_labels_to(fmt::memory_buffer& buffer) const {
        for (const auto& param : parameters_) {
            buffer.push_back(' ');
            param->format_label_to(buffer);
        }
    }

    /*!
     * \brief Format a line of values.
     *
     * \param[out] buffer Buffer to format to.
     */
    void format_values_to(fmt::memory_buffer& buffer) const {
        for (const auto& param : parameters_) {
            buffer.push_back(' ');
            param->format_value_to(buffer);
        }
    }

    /*!
     * \brief Format a line of values.
     *
     * \param[out] buffer Buffer to format to.
     * \param[in] algorithm Algorithm.
     */
    void format_values_to(
        fmt::memory_buffer& buffer, Algorithm* algorithm) const {
        for (const auto& param : parameters_) {
            buffer.push_back(' ');
            param->format_value_to(buffer, algorithm);
        }
    }

    /*!
     * \brief Format a line of summary.
     *
     * \param[out] buffer Buffer to format to.
     */
    void format_summary_to(fmt::memory_buffer& buffer) const {
        buffer.append(std::string_view("Finished iterations: "));
        bool is_first = true;
        for (const auto& param : parameters_) {
            if (is_first) {
                is_first = false;
            } else {
                buffer.push_back(',');
                buffer.push_back(' ');
            }
            param->format_summary_to(buffer);
        }
    }

    /*!
     * \brief Format a line of summary.
     *
     * \param[out] buffer Buffer to format to.
     * \param[in] algorithm Algorithm.
     */
    void format_summary_to(
        fmt::memory_buffer& buffer, Algorithm* algorithm) const {
        buffer.append(std::string_view("Finished iterations: "));
        bool is_first = true;
        for (const auto& param : parameters_) {
            if (is_first) {
                is_first = false;
            } else {
                buffer.push_back(',');
                buffer.push_back(' ');
            }
            param->format_summary_to(buffer, algorithm);
        }
    }

    //! Log tag.
    log_tag_view tag_;

    //! Whether to write iteration logs.
    bool write_iterations_{false};

    //! Whether to write summary logs.
    bool write_summaries_{false};

    //! Log sink.
    std::shared_ptr<sinks::log_sink_base> sink_;

    //! Checker of periods to write iteration logs.
    util::iteration_period_checker iteration_output_period_checker_;

    //! Checker of periods to write labels of iteration logs.
    util::iteration_period_checker iteration_label_period_checker_;

    //! Parameters.
    std::vector<std::shared_ptr<iteration_parameter_base<Algorithm>>>
        parameters_{};

    //! Buffer of logging data.
    fmt::memory_buffer buffer_{};
};

}  // namespace num_collect::logging::iterations
