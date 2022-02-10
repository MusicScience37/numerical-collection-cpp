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
 * \brief Definition of assertion_decomposer class.
 */
#pragma once

#include <iterator>
#include <type_traits>

#include <fmt/format.h>

#include "num_collect/base/concepts/convertible_to.h"
#include "num_collect/base/concepts/formattable.h"
#include "num_collect/util/comparators.h"
#include "num_collect/util/concepts/comparator.h"
#include "num_collect/util/concepts/rhs_comparable.h"

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
 * \tparam Value Type of the value.
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
    explicit assertion_value(const Value& value) : value_(&value) {}

    /*!
     * \brief Get the value.
     *
     * \return Value.
     */
    [[nodiscard]] auto value() const noexcept -> const Value& {
        return *value_;
    }

    /*!
     * \brief Evaluate and get boolean result.
     *
     * \return Result.
     */
    [[nodiscard]] auto evaluate_to_bool() const noexcept -> bool {
        static_assert(base::concepts::convertible_to<Value, bool>);
        return static_cast<bool>(*value_);
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
            return fmt::format_to(out, FMT_STRING("{}"), *value_);
        } else {
            return fmt::format_to(
                out, FMT_STRING("{}"), "<unformattable-value>");
        }
    }

private:
    //! Value.
    const Value* value_;
};

/*!
 * \brief Class of comparisons in assertions.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Right Type of the right-hand-side value.
 * \tparam Comparator Type of the function object to compare two values.
 */
template <typename Left, typename Right,
    concepts::comparator<Left, Right> Comparator>
class assertion_comparison
    : public assertion_expression_base<
          assertion_comparison<Left, Right, Comparator>> {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] left Left-hand-side value.
     * \param[in] right Right-hand-side value.
     * \param[in] operator_str String expression of the operator.
     * \param[in] comparator Function object to compare two values.
     */
    assertion_comparison(assertion_value<Left> left,
        assertion_value<Right> right, std::string_view operator_str,
        Comparator comparator = Comparator())
        : left_(left),
          right_(right),
          operator_str_(operator_str),
          comparator_(comparator) {}

    /*!
     * \brief Get the right-hand-side value.
     *
     * \return Right-hand-side value.
     */
    [[nodiscard]] auto right() const noexcept -> const assertion_value<Right>& {
        return right_;
    }

    /*!
     * \brief Evaluate and get boolean result.
     *
     * \return Result.
     */
    [[nodiscard]] auto evaluate_to_bool() const noexcept -> bool {
        return comparator_(left_.value(), right_.value());
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
        out = left_.format_to(out);
        out = fmt::format_to(out, " {} ", operator_str_);
        return right_.format_to(out);
    }

private:
    //! Left-hand-side value.
    assertion_value<Left> left_;

    //! Right-hand-side value.
    assertion_value<Right> right_;

    //! String expression of the operator.
    std::string_view operator_str_;

    //! Function object to compare two values.
    Comparator comparator_;
};

/*!
 * \brief Create an object to compare two values.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Right Type of the right-hand-side value.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Object to compare the two values.
 */
template <typename Left, concepts::rhs_less_than_comparable<Left> Right>
[[nodiscard]] inline auto operator<(
    const assertion_value<Left>& left, const Right& right) {
    return assertion_comparison<Left, Right, less<Left, Right>>(
        left, assertion_value<Right>(right), "<");
}

/*!
 * \brief Create an object to compare two values.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Right Type of the right-hand-side value.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Object to compare the two values.
 */
template <typename Left,
    concepts::rhs_less_than_or_equal_to_comparable<Left> Right>
[[nodiscard]] inline auto operator<=(
    const assertion_value<Left>& left, const Right& right) {
    return assertion_comparison<Left, Right, less_equal<Left, Right>>(
        left, assertion_value<Right>(right), "<=");
}

/*!
 * \brief Create an object to compare two values.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Right Type of the right-hand-side value.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Object to compare the two values.
 */
template <typename Left, concepts::rhs_greater_than_comparable<Left> Right>
[[nodiscard]] inline auto operator>(
    const assertion_value<Left>& left, const Right& right) {
    return assertion_comparison<Left, Right, greater<Left, Right>>(
        left, assertion_value<Right>(right), ">");
}

/*!
 * \brief Create an object to compare two values.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Right Type of the right-hand-side value.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Object to compare the two values.
 */
template <typename Left,
    concepts::rhs_greater_than_or_equal_to_comparable<Left> Right>
[[nodiscard]] inline auto operator>=(
    const assertion_value<Left>& left, const Right& right) {
    return assertion_comparison<Left, Right, greater_equal<Left, Right>>(
        left, assertion_value<Right>(right), ">=");
}

/*!
 * \brief Create an object to compare two values.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Right Type of the right-hand-side value.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Object to compare the two values.
 */
template <typename Left, concepts::rhs_equal_to_comparable<Left> Right>
[[nodiscard]] inline auto operator==(
    const assertion_value<Left>& left, const Right& right) {
    return assertion_comparison<Left, Right, equal<Left, Right>>(
        left, assertion_value<Right>(right), "==");
}

/*!
 * \brief Create an object to compare two values.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Right Type of the right-hand-side value.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Object to compare the two values.
 */
template <typename Left, concepts::rhs_not_equal_to_comparable<Left> Right>
[[nodiscard]] inline auto operator!=(
    const assertion_value<Left>& left, const Right& right) {
    return assertion_comparison<Left, Right, not_equal<Left, Right>>(
        left, assertion_value<Right>(right), "!=");
}

/*!
 * \brief Class to compare three values.
 *
 * \tparam Left Type of the left-hand-side value.
 * \tparam Middle Type of the middle value.
 * \tparam Right Type of the right-hand-side value.
 * \tparam ComparatorLeft Type of the function object to compare left two
 * values.
 * \tparam ComparatorRight Type of the function object to compare right
 * two values.
 */
template <typename Left, typename Middle, typename Right,
    concepts::comparator<Left, Middle> ComparatorLeft,
    concepts::comparator<Middle, Right> ComparatorRight>
class assertion_comparison2
    : public assertion_expression_base<assertion_comparison2<Left, Middle,
          Right, ComparatorLeft, ComparatorRight>> {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] comp_left Comparison of two values.
     * \param[in] right Right-hand-side value.
     * \param[in] operator_str String expression of the operator.
     * \param[in] comparator Function object to compare two values.
     */
    assertion_comparison2(
        assertion_comparison<Left, Middle, ComparatorLeft> comp_left,
        assertion_value<Right> right, std::string_view operator_str,
        ComparatorRight comparator = ComparatorRight())
        : comp_left_(comp_left),
          right_(right),
          operator_str_(operator_str),
          comparator_(comparator) {}

    /*!
     * \brief Evaluate and get boolean result.
     *
     * \return Result.
     */
    [[nodiscard]] auto evaluate_to_bool() const noexcept -> bool {
        return comp_left_.evaluate_to_bool() &&
            comparator_(comp_left_.right().value(), right_.value());
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
        out = comp_left_.format_to(out);
        out = fmt::format_to(out, " {} ", operator_str_);
        return right_.format_to(out);
    }

private:
    //! Comparison of two values.
    assertion_comparison<Left, Middle, ComparatorLeft> comp_left_;

    //! Right-hand-side value.
    assertion_value<Right> right_;

    //! String expression of the operator.
    std::string_view operator_str_;

    //! Function object to compare two values.
    ComparatorRight comparator_;
};

/*!
 * \brief Class to decompose comprisons in assertions.
 */
class assertion_decomposer {
public:
    /*!
     * \brief Construct.
     */
    assertion_decomposer() = default;

    /*!
     * \brief Create a value in assertions.
     *
     * \tparam Value Type of the value.
     * \param[in] value Value.
     * \return Value object.
     */
    template <typename Value>
    [[nodiscard]] auto operator<(const Value& value) {
        return assertion_value<Value>(value);
    }
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
