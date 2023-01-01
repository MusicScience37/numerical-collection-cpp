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
 * \brief Definition of iteration_parameter class.
 */
#pragma once

#include <iterator>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/concepts/formattable_iteration_parameter_value.h"  // IWYU pragma: keep
#include "num_collect/logging/concepts/iteration_parameter_value.h"  // IWYU pragma: keep
#include "num_collect/logging/iterations/iteration_parameter_formatter.h"  // IWYU pragma: keep
#include "num_collect/logging/iterations/iteration_parameter_formatter_decl.h"

namespace num_collect::logging::iterations {

namespace impl {

//! Default width in .num_collect::logging::iterations::iteration_parameter.
inline constexpr index_type iteration_parameter_default_width = 11;

}  // namespace impl

/*!
 * \brief Base class of parameters in iterations.
 *
 * \tparam Algorithm Type of the algorithm.
 *
 * \note User should not use this class directly.
 * Use num_collect::logging::iterations::iteration_logger.
 */
template <typename Algorithm>
class iteration_parameter_base {
public:
    /*!
     * \brief Format the label with alignment.
     *
     * \param[out] buffer Buffer to write the output.
     */
    virtual void format_label_to(fmt::memory_buffer& buffer) const = 0;

    /*!
     * \brief Format the value with alignment.
     *
     * \param[out] buffer Buffer to write the output.
     */
    virtual void format_value_to(fmt::memory_buffer& buffer) const = 0;

    /*!
     * \brief Format the summary.
     *
     * \param[out] buffer Buffer to write the output.
     */
    virtual void format_summary_to(fmt::memory_buffer& buffer) const = 0;

    /*!
     * \brief Format the value with alignment.
     *
     * \param[out] buffer Buffer to write the output.
     * \param[in] algorithm Algorithm.
     */
    virtual void format_value_to(
        fmt::memory_buffer& buffer, Algorithm* algorithm) const = 0;

    /*!
     * \brief Format the summary.
     *
     * \param[out] buffer Buffer to write the output.
     * \param[in] algorithm Algorithm.
     */
    virtual void format_summary_to(
        fmt::memory_buffer& buffer, Algorithm* algorithm) const = 0;

    /*!
     * \brief Get the label of this parameter.
     *
     * \return label.
     */
    [[nodiscard]] virtual auto label() const noexcept -> const std::string& = 0;

    iteration_parameter_base(const iteration_parameter_base&) = delete;
    iteration_parameter_base(iteration_parameter_base&&) = delete;
    auto operator=(const iteration_parameter_base&)
        -> iteration_parameter_base& = delete;
    auto operator=(iteration_parameter_base&&)
        -> iteration_parameter_base& = delete;

    /*!
     * \brief Destructor.
     */
    virtual ~iteration_parameter_base() noexcept = default;

protected:
    /*!
     * \brief Constructor.
     */
    iteration_parameter_base() noexcept = default;
};

/*!
 * \brief Class of parameters in iterations.
 *
 * \tparam Algorithm Type of the algorithm.
 * \tparam Value Type of values.
 * \tparam ParameterValue Class of parameters values in iterations
 * specified by variables.
 *
 * \note User should not use this class directly.
 * Use num_collect::logging::iterations::iteration_logger.
 *
 * \thread_safety Not thread-safe.
 */
template <typename Algorithm,
    concepts::formattable_iteration_parameter_value Value,
    concepts::iteration_parameter_value<Algorithm, Value> ParameterValue>
class iteration_parameter final : public iteration_parameter_base<Algorithm> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] label Label.
     * \param[in] value Parameter value.
     */
    iteration_parameter(std::string label, ParameterValue value)
        : label_(std::move(label)), value_(std::move(value)) {}

    /*!
     * \brief Format the label with alignment.
     *
     * \param[out] buffer Buffer to write the output.
     */
    void format_label_to(fmt::memory_buffer& buffer) const override {
        fmt::format_to(std::back_inserter(buffer), "{0: >{1}}", label_, width_);
    }

    /*!
     * \brief Format the value with alignment.
     *
     * \param[out] buffer Buffer to write the output.
     */
    void format_value_to(fmt::memory_buffer& buffer) const override {
        formatter_.format_with_alignment(value_.get(), width_, buffer);
    }

    /*!
     * \brief Format the summary.
     *
     * \param[out] buffer Buffer to write the output.
     */
    void format_summary_to(fmt::memory_buffer& buffer) const override {
        buffer.append(label_);
        buffer.push_back('=');
        formatter_.format(value_.get(), buffer);
    }

    /*!
     * \brief Format the value with alignment.
     *
     * \param[out] buffer Buffer to write the output.
     * \param[in] algorithm Algorithm.
     */
    void format_value_to(
        fmt::memory_buffer& buffer, Algorithm* algorithm) const override {
        formatter_.format_with_alignment(value_.get(algorithm), width_, buffer);
    }

    /*!
     * \brief Format the summary.
     *
     * \param[out] buffer Buffer to write the output.
     * \param[in] algorithm Algorithm.
     */
    void format_summary_to(
        fmt::memory_buffer& buffer, Algorithm* algorithm) const override {
        buffer.append(label_);
        buffer.push_back('=');
        formatter_.format(value_.get(algorithm), buffer);
    }

    /*!
     * \brief Get the label of this parameter.
     *
     * \return label.
     */
    [[nodiscard]] auto label() const noexcept -> const std::string& override {
        return label_;
    }

    /*!
     * \brief Set width.
     *
     * \param[in] value Value
     * \return This object.
     */
    auto width(index_type value) -> iteration_parameter* {
        if (value <= 0) [[unlikely]] {
            throw invalid_argument("Width must be a positive number.");
        }
        width_ = value;
        return this;
    }

    /*!
     * \brief Get width.
     *
     * \return Width.
     */
    [[nodiscard]] auto width() const -> index_type { return width_; }

    /*!
     * \brief Access the formatter.
     *
     * \return Formatter.
     */
    [[nodiscard]] auto formatter() -> iteration_parameter_formatter<Value>& {
        return formatter_;
    }

private:
    //! Label.
    std::string label_;

    //! Parameter value.
    ParameterValue value_;

    //! Width.
    index_type width_{impl::iteration_parameter_default_width};

    //! Formatter.
    iteration_parameter_formatter<Value> formatter_{};
};

}  // namespace num_collect::logging::iterations
