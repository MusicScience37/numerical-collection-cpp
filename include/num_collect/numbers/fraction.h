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

namespace impl {

/*!
 * \brief Compute the common divisor of two integers for fractions.
 *
 * \tparam Integer Type of integers.
 * \param[in] a Integer.
 * \param[in] b Integer.
 * \return Common divisor.
 */
template <base::concepts::integral Integer>
[[nodiscard]] static constexpr auto common_divisor_for_fraction(
    Integer a, Integer b) {
    if (a == static_cast<Integer>(0)) {
        if (b == static_cast<Integer>(0)) {
            return static_cast<Integer>(1);
        }
        return b;
    }
    if (b == static_cast<Integer>(0)) {
        return a;
    }

    if constexpr (std::is_signed_v<Integer>) {
        if (a < static_cast<Integer>(0)) {
            a = -a;
        }
        if (b < static_cast<Integer>(0)) {
            b = -b;
        }
    }

    return util::greatest_common_divisor(a, b);
}

}  // namespace impl

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
     * \brief Normalize this fraction.
     */
    constexpr void normalize() {
        if constexpr (std::is_signed_v<integer_type>) {
            if (denominator_ < static_cast<integer_type>(0)) {
                numerator_ = -numerator_;
                denominator_ = -denominator_;
            }
        }

        const auto common_divisor =
            impl::common_divisor_for_fraction(numerator_, denominator_);
        numerator_ /= common_divisor;
        denominator_ /= common_divisor;
    }

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
    auto operator+=(const fraction& right) -> fraction& {
        add_safely(right);
        return *this;
    }

    /*!
     * \brief Add a fraction to this fraction.
     *
     * \param[in] right Right-hand-side object.
     * \return This fraction after addition.
     */
    auto add_safely(const fraction& right) -> fraction& {
        if (denominator_ == right.denominator_) {
            numerator_ += right.numerator_;
        } else {
            NUM_COLLECT_DEBUG_ASSERT(
                denominator_ > static_cast<integer_type>(0));
            NUM_COLLECT_DEBUG_ASSERT(
                right.denominator_ > static_cast<integer_type>(0));
            const integer_type common_divisor =
                util::greatest_common_divisor(denominator_, right.denominator_);
            const integer_type right_coeff = denominator_ / common_divisor;
            const integer_type my_coeff = right.denominator_ / common_divisor;

            denominator_ = util::multiply_safely(denominator_, my_coeff);
            NUM_COLLECT_DEBUG_ASSERT(
                right_coeff * right.denominator_ == denominator_);

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
     */
    auto operator==(const fraction& right) const -> bool {
        if (denominator_ == right.denominator_) {
            return numerator_ == right.numerator_;
        }
        NUM_COLLECT_DEBUG_ASSERT(denominator_ > static_cast<integer_type>(0));
        NUM_COLLECT_DEBUG_ASSERT(
            right.denominator_ > static_cast<integer_type>(0));
        const auto common_divisor =
            util::greatest_common_divisor(denominator_, right.denominator_);
        const integer_type right_coeff = denominator_ / common_divisor;
        const integer_type my_coeff = right.denominator_ / common_divisor;
        return util::multiply_safely(numerator_, my_coeff) ==
            util::multiply_safely(right.numerator_, right_coeff);
    }

private:
    //! Numerator.
    integer_type numerator_;

    //! Denominator.
    integer_type denominator_;
};

}  // namespace num_collect::numbers
