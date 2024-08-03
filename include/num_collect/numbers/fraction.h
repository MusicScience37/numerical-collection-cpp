/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of fraction class.
 */
#pragma once

#include <ostream>
#include <type_traits>  // IWYU pragma: keep

#include <fmt/format.h>
#include <fmt/ostream.h>  // IWYU pragma: keep

#include "num_collect/base/concepts/integral.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/greatest_common_divisor.h"
#include "num_collect/util/multiply_safely.h"

namespace num_collect::numbers {

/*!
 * \brief Class of fractions.
 *
 * \tparam IntegerType Type of integers.
 */
template <base::concepts::integral IntegerType>
class fraction {
public:
    //! Type of integers.
    using integer_type = IntegerType;

    /*!
     * \brief Constructor.
     *
     * \param[in] numerator Numerator.
     * \param[in] denominator Denominator.
     */
    constexpr fraction(integer_type numerator, integer_type denominator)
        : numerator_(numerator), denominator_(denominator) {
        if (denominator_ == static_cast<integer_type>(0)) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "The denominator of a fraction must not be zero.");
        }
        normalize();
    }

    /*!
     * \brief Constructor. (Convert from an integer.)
     *
     * \param[in] integer Integer.
     */
    constexpr fraction(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        integer_type integer) noexcept
        : numerator_(integer), denominator_(static_cast<integer_type>(1)) {}

    /*!
     * \brief Constructor. (Initialize to zero.)
     */
    constexpr fraction() noexcept : fraction(static_cast<integer_type>(0)) {}

    /*!
     * \brief Get the numerator.
     *
     * \return Numerator.
     */
    [[nodiscard]] constexpr auto numerator() const noexcept -> integer_type {
        return numerator_;
    }

    /*!
     * \brief Get the denominator.
     *
     * \return Denominator.
     */
    [[nodiscard]] constexpr auto denominator() const noexcept -> integer_type {
        return denominator_;
    }

    /*!
     * \brief Add a fraction to this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after addition.
     */
    constexpr auto operator+=(const fraction& right) -> fraction& {
        add(right);
        return *this;
    }

    /*!
     * \brief Add a fraction to this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after addition.
     */
    constexpr auto add(const fraction& right) -> fraction& {
        if (denominator_ == right.denominator_) {
            // TODO: Add safely.
            numerator_ += right.numerator_;
        } else {
            const integer_type common_divisor =
                util::greatest_common_divisor(denominator_, right.denominator_);
            const integer_type right_coeff = denominator_ / common_divisor;
            const integer_type my_coeff = right.denominator_ / common_divisor;

            denominator_ = util::multiply_safely(denominator_, my_coeff);

            // TODO: Add safely.
            numerator_ = util::multiply_safely(numerator_, my_coeff) +
                util::multiply_safely(right.numerator_, right_coeff);
        }
        normalize();
        return *this;
    }

    /*!
     * \brief Subtract a fraction from this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after subtraction.
     */
    constexpr auto operator-=(const fraction& right) -> fraction& {
        subtract(right);
        return *this;
    }

    /*!
     * \brief Subtract a fraction from this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after subtraction.
     */
    constexpr auto subtract(const fraction& right) -> fraction& {
        if (denominator_ == right.denominator_) {
            // TODO: Subtract safely.
            numerator_ -= right.numerator_;
        } else {
            const integer_type common_divisor =
                util::greatest_common_divisor(denominator_, right.denominator_);
            const integer_type right_coeff = denominator_ / common_divisor;
            const integer_type my_coeff = right.denominator_ / common_divisor;

            denominator_ = util::multiply_safely(denominator_, my_coeff);

            // TODO: Subtract safely.
            numerator_ = util::multiply_safely(numerator_, my_coeff) -
                util::multiply_safely(right.numerator_, right_coeff);
        }
        normalize();
        return *this;
    }

    /*!
     * \brief Multiply a fraction to this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after multiplication.
     */
    constexpr auto operator*=(const fraction& right) -> fraction& {
        multiply(right);
        return *this;
    }

    /*!
     * \brief Multiply a fraction to this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after multiplication.
     */
    constexpr auto multiply(const fraction& right) -> fraction& {
        numerator_ = util::multiply_safely(numerator_, right.numerator_);
        denominator_ = util::multiply_safely(denominator_, right.denominator_);
        normalize();
        return *this;
    }

    /*!
     * \brief Divide this fraction by a fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after division.
     */
    constexpr auto operator/=(const fraction& right) -> fraction& {
        divide_by(right);
        return *this;
    }

    /*!
     * \brief Divide this fraction by a fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after division.
     */
    constexpr auto divide_by(const fraction& right) -> fraction& {
        multiply(right.inverse());
        return *this;
    }

    /*!
     * \brief Get the inverse of this fraction.
     *
     * \return Inverse.
     */
    [[nodiscard]] constexpr auto inverse() const -> fraction {
        return fraction<integer_type>(denominator_, numerator_);
    }

    /*!
     * \brief Compare this fraction with another fraction.
     *
     * \param[in] right Right-hand-side object.
     * \retval true This fraction is same with the given fraction.
     * \retval false This fraction is different from the given fraction.
     */
    constexpr auto operator==(const fraction& right) const noexcept -> bool {
        // This fraction is always normalized, so simple comparison is enough.
        return denominator_ == right.denominator_ &&
            numerator_ == right.numerator_;
    }

    /*!
     * \brief Compare this fraction with another fraction.
     *
     * \param[in] right Right-hand-side object.
     * \retval true This fraction is different from the given fraction.
     * \retval false This fraction is same with the given fraction.
     */
    constexpr auto operator!=(const fraction& right) const noexcept -> bool {
        return !operator==(right);
    }

private:
    /*!
     * \brief Normalize this fraction.
     */
    constexpr void normalize() {
        if constexpr (std::is_signed_v<integer_type>) {
            if (denominator_ < static_cast<integer_type>(0)) {
                numerator_ = -numerator_;
                denominator_ = -denominator_;
            }
        }

        if (numerator_ == static_cast<integer_type>(0)) {
            denominator_ = static_cast<integer_type>(1);
            return;
        }

        const auto common_divisor =
            util::greatest_common_divisor(int_abs(numerator_), denominator_);
        numerator_ /= common_divisor;
        denominator_ /= common_divisor;
    }

    /*!
     * \brief Get the absolute value of an integer.
     *
     * \param[in] x Integer.
     * \return Absolute value.
     */
    static constexpr auto int_abs(integer_type x) noexcept -> integer_type {
        // TODO: move to utility or base.
        if constexpr (std::is_unsigned_v<integer_type>) {
            return x;
        } else {
            if (x < static_cast<integer_type>(0)) {
                return -x;
            }
            return x;
        }
    }

    //! Numerator.
    integer_type numerator_;

    //! Denominator.
    integer_type denominator_;
};

/*!
 * \brief Add two fractions.
 *
 * \tparam Integer Type of integers in the fractions.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Sum of the fractions.
 */
template <num_collect::base::concepts::integral Integer>
constexpr auto operator+(const fraction<Integer>& left,
    const fraction<Integer>& right) -> fraction<Integer> {
    return fraction<Integer>(left) += right;
}

/*!
 * \brief Subtract a fraction from another fraction.
 *
 * \tparam Integer Type of integers in the fractions.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Difference of the fractions.
 */
template <num_collect::base::concepts::integral Integer>
constexpr auto operator-(const fraction<Integer>& left,
    const fraction<Integer>& right) -> fraction<Integer> {
    return fraction<Integer>(left) -= right;
}

/*!
 * \brief Multiply two fractions.
 *
 * \tparam Integer Type of integers in the fractions.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Product of the fractions.
 */
template <num_collect::base::concepts::integral Integer>
constexpr auto operator*(const fraction<Integer>& left,
    const fraction<Integer>& right) -> fraction<Integer> {
    return fraction<Integer>(left) *= right;
}

/*!
 * \brief Divide a fraction by another fraction.
 *
 * \tparam Integer Type of integers in the fractions.
 * \param[in] left Left-hand-side value.
 * \param[in] right Right-hand-side value.
 * \return Quotient of the fractions.
 */
template <num_collect::base::concepts::integral Integer>
constexpr auto operator/(const fraction<Integer>& left,
    const fraction<Integer>& right) -> fraction<Integer> {
    return fraction<Integer>(left) /= right;
}

}  // namespace num_collect::numbers

namespace fmt {

/*!
 * \brief fmt::formatter for num_collect::numbers::fraction.
 *
 * \tparam Integer Type of integers in fractions.
 */
template <num_collect::base::concepts::integral Integer>
struct formatter<num_collect::numbers::fraction<Integer>> {
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
    auto format(num_collect::numbers::fraction<Integer> val,
        FormatContext& context) const {
        return fmt::format_to(
            context.out(), "{} / {}", val.numerator(), val.denominator());
    }
};

}  // namespace fmt

namespace num_collect::numbers {

/*!
 * \brief Format a fraction.
 *
 * \tparam Integer Type of integers in the fraction.
 * \param[in] stream Output stream.
 * \param[in] val Fraction value.
 * \return Output stream.
 */
template <num_collect::base::concepts::integral Integer>
inline auto operator<<(
    std::ostream& stream, const fraction<Integer>& val) -> std::ostream& {
    fmt::print(stream, "{}", val);
    return stream;
}

}  // namespace num_collect::numbers
