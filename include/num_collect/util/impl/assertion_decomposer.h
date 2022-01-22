/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of assertion_decomposer class.
 */
#pragma once

#include <iterator>
#include <type_traits>

#include <fmt/format.h>

#include "num_collect/base/concepts/convertible_to.h"
#include "num_collect/base/concepts/formattable.h"

namespace num_collect::util::impl {

/*!
 * \brief Base class of expressions in assertions.
 *
 * \tparam Derived Type of the derived class.
 */
template <typename Derived>
class assertion_expression_base {
public:
    /*!
     * \brief Evaluate and get boolean result.
     *
     * \return Result.
     */
    [[nodiscard]] auto evaluate_to_bool() const noexcept -> bool {
        return derived().evaluate_to_bool();
    }

    /*!
     * \brief Format expression.
     *
     * \tparam OutputIterator Type of the output iterator.
     * \param[in] out Output iterator to format to.
     * \return Output iterator after formatting.
     */
    template <typename OutputIterator>
    [[nodiscard]] auto format_to(OutputIterator out) const -> OutputIterator {
        derived().format_to(out);
    }

protected:
    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }
};

/*!
 * \brief Class of values in assertions.
 *
 * \tparam Value Value.
 */
template <typename Value>
class assertion_value
    : public assertion_expression_base<assertion_value<Value>> {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] value Value.
     */
    explicit assertion_value(const Value& value) : value_(value) {}

    /*!
     * \brief Get the value.
     *
     * \return Value.
     */
    [[nodiscard]] auto value() const noexcept -> const Value& { return value_; }

    /*!
     * \brief Evaluate and get boolean result.
     *
     * \return Result.
     */
    [[nodiscard]] auto evaluate_to_bool() const noexcept -> bool {
        static_assert(base::concepts::convertible_to<Value, bool>);
        return static_cast<bool>(value_);
    }

    /*!
     * \brief Format expression.
     *
     * \tparam OutputIterator Type of the output iterator.
     * \param[in] out Output iterator to format to.
     * \return Output iterator after formatting.
     */
    template <typename OutputIterator>
    [[nodiscard]] auto format_to(OutputIterator out) const -> OutputIterator {
        if constexpr (base::concepts::formattable<Value>) {
            return fmt::format_to(out, FMT_STRING("{}"), value_);
        } else {
            return fmt::format_to(
                out, FMT_STRING("{}"), "<unformattable-value>");
        }
    }

private:
    //! Value.
    const Value& value_;
};

}  // namespace num_collect::util::impl

namespace fmt {

/*!
 * \brief fmt::formatter for classes derived from
 * num_collect::util::impl::assertion_expression_base.
 */
template <typename T>
struct formatter<T, char,
    std::enable_if_t<std::is_base_of_v<
        num_collect::util::impl::assertion_expression_base<T>, T>>> {
public:
    /*!
     * \brief Parse format specifications.
     *
     * \param[in] context Context.
     * \return Iterator.
     */
    constexpr auto parse(format_parse_context& context)  // NOLINT
        -> decltype(context.begin()) {
        return context.end();
    }

    /*!
     * \brief Format a value.
     *
     * \tparam FormatContext Type of the context.
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    template <typename FormatContext>
    auto format(const T& val, FormatContext& context) {
        return val.format_to(context.out());
    }
};

}  // namespace fmt
