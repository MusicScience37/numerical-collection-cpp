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

#include <type_traits>

#include "num_collect/base/concepts/integral.h"
#include "num_collect/base/exception.h"
#include "num_collect/util/assert.h"
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
            throw invalid_argument(
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
        add_safely(right);
        return *this;
    }

    /*!
     * \brief Add a fraction to this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after addition.
     */
    constexpr auto add_safely(const fraction& right) -> fraction& {
        if (denominator_ == right.denominator_) {
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

}  // namespace num_collect::numbers
