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
 * \brief Definition of iteration_parameter_formatter class.
 */
#pragma once

#include <iterator>
#include <optional>
#include <string_view>

#include <fmt/format.h>

#include "num_collect/base/concepts/formattable.h"  // IWYU pragma: keep
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/concepts/formattable_iteration_parameter_value.h"  // IWYU pragma: keep
#include "num_collect/logging/concepts/formattable_real_scalar.h"  // IWYU pragma: keep
#include "num_collect/logging/iterations/iteration_paramter_formatter_decl.h"

namespace num_collect::logging::iterations {

namespace impl {

/*!
 * \brief Default precision of floating-point values in
 * num_collect::logging::iterations::iteration_parameter_formatter.
 */
inline constexpr index_type iteration_parameter_formatter_default_precision = 4;

}  // namespace impl

/*!
 * \brief Class of the formatter of parameter values in iterations.
 *
 * \tparam Value Type of values.
 *
 * \thread_safety Not thread-safe.
 */
template <base::concepts::formattable Value>
class iteration_parameter_formatter<Value> {
public:
    /*!
     * \brief Constructor.
     */
    iteration_parameter_formatter() noexcept = default;

    /*!
     * \brief Format a value.
     *
     * \param[in] value Value.
     * \param[out] buffer Buffer to write the formatted value.
     */
    void format(const Value& value, fmt::memory_buffer& buffer) const {
        fmt::format_to(std::back_inserter(buffer), "{0}", value);
    }

    /*!
     * \brief Format a value with alignment.
     *
     * \param[in] value Value.
     * \param[in] width Width of the output.
     * \param[out] buffer Buffer to write the formatted value.
     */
    void format_with_alignment(const Value& value, index_type width,
        fmt::memory_buffer& buffer) const {
        fmt::format_to(std::back_inserter(buffer), "{0: >{1}}", value, width);
    }
};

/*!
 * \brief Class of the formatter of parameter values in iterations.
 *
 * \tparam Value Type of values.
 *
 * \thread_safety Not thread-safe.
 */
template <concepts::formattable_real_scalar Value>
class iteration_parameter_formatter<Value> {
public:
    /*!
     * \brief Constructor.
     */
    iteration_parameter_formatter() noexcept = default;

    /*!
     * \brief Format a value.
     *
     * \param[in] value Value.
     * \param[out] buffer Buffer to write the formatted value.
     */
    void format(const Value& value, fmt::memory_buffer& buffer) const {
        fmt::format_to(
            std::back_inserter(buffer), "{0:.{1}}", value, precision_);
    }

    /*!
     * \brief Format a value with alignment.
     *
     * \param[in] value Value.
     * \param[in] width Width of the output.
     * \param[out] buffer Buffer to write the formatted value.
     */
    void format_with_alignment(const Value& value, index_type width,
        fmt::memory_buffer& buffer) const {
        fmt::format_to(std::back_inserter(buffer), "{0: >{1}.{2}}", value,
            width, precision_);
    }

    /*!
     * \brief Set precision.
     *
     * \param[in] value Value
     * \return This object.
     */
    auto precision(index_type value) -> iteration_parameter_formatter& {
        if (value <= 0) {
            throw invalid_argument("Precision must be a positive number.");
        }
        precision_ = value;
        return *this;
    }

    /*!
     * \brief Get precision.
     *
     * \return Precision.
     */
    [[nodiscard]] auto precision() const -> index_type { return precision_; }

private:
    //! Precision.
    index_type precision_{
        impl::iteration_parameter_formatter_default_precision};
};

/*!
 * \brief Class of the formatter of parameter values in iterations.
 *
 * \tparam Value Type of values.
 *
 * \thread_safety Not thread-safe.
 */
template <concepts::formattable_iteration_parameter_value Value>
class iteration_parameter_formatter<std::optional<Value>>
    : public iteration_parameter_formatter<Value> {
public:
    /*!
     * \brief Constructor.
     */
    iteration_parameter_formatter() noexcept = default;

    /*!
     * \brief Format a value.
     *
     * \param[in] value Value.
     * \param[out] buffer Buffer to write the formatted value.
     */
    void format(
        const std::optional<Value>& value, fmt::memory_buffer& buffer) const {
        if (value) {
            iteration_parameter_formatter<Value>::format(*value, buffer);
        } else {
            buffer.append(null_string);
        }
    }

    /*!
     * \brief Format a value with alignment.
     *
     * \param[in] value Value.
     * \param[in] width Width of the output.
     * \param[out] buffer Buffer to write the formatted value.
     */
    void format_with_alignment(const std::optional<Value>& value,
        index_type width, fmt::memory_buffer& buffer) const {
        if (value) {
            iteration_parameter_formatter<Value>::format_with_alignment(
                *value, width, buffer);
        } else {
            fmt::format_to(
                std::back_inserter(buffer), "{0: >{1}}", null_string, width);
        }
    }

private:
    //! String to express null.
    static constexpr std::string_view null_string{"---"};
};

}  // namespace num_collect::logging::iterations
