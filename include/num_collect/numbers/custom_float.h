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
 * \brief Definition of custom_float class.
 */
#pragma once

#include <cstdint>
#include <limits>

#include "num_collect/base/concepts/integral.h"         // IWYU pragma: keep
#include "num_collect/base/concepts/signed_integral.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/util/bidirectional_vector.h"

namespace num_collect::numbers {

/*!
 * \brief Class of floating numbers in a custom number system with given radix.
 *
 * \tparam Radix Radix of the number system.
 * \tparam Digit Type of digits.
 * \tparam DigitCalc Type of digits for calculation.
 */
template <index_type Radix, base::concepts::integral Digit = std::uint8_t,
    base::concepts::signed_integral DigitCalc = std::int32_t>
class custom_float {
public:
    static_assert(std::numeric_limits<Digit>::max() > Radix);
    static_assert(std::numeric_limits<DigitCalc>::max() > Radix * Radix);
    static_assert(std::numeric_limits<DigitCalc>::min() < -Radix * Radix);

    //! Radix of the number system.
    static constexpr index_type radix = Radix;

    //! Type of digits.
    using digit_type = Digit;

    //! Type of digits for calculation.
    using digit_calc_type = DigitCalc;

    /*!
     * \brief Construct zero.
     */
    custom_float()
        : digits_(
              typename util::bidirectional_vector<digit_type>::container_type{
                  0},
              0) {}

    /*!
     * \brief Get the index of the lowest digits
     *
     * \return Index of lowest digits.
     */
    [[nodiscard]] auto lowest_ind() const noexcept -> index_type {
        return digits_.min_index();
    }

    /*!
     * \brief Get the index of the highest digit.
     *
     * \return Index of the highest digit.
     */
    [[nodiscard]] auto highest_ind() const noexcept -> index_type {
        return digits_.max_index();
    }

    /*!
     * \brief Access a digit with checks.
     *
     * \param[in] index Index.
     * \return Index.
     */
    [[nodiscard]] auto at(index_type index) const -> digit_type {
        return digits_.at(index);
    }

    /*!
     * \brief Access a digit without checks.
     *
     * \warning Access to indices out of range causes undefined behaviour.
     *
     * \param[in] index Index.
     * \return Index.
     */
    [[nodiscard]] auto operator[](index_type index) const -> digit_type {
        return digits_[index];
    }

    /*!
     * \brief Access a digit preparing it if needed.
     *
     * \warning Inserted value won't be checked.
     *
     * \param[in] index Index.
     * \return Index.
     */
    [[nodiscard]] auto operator[](index_type index) -> digit_type& {
        return digits_.get_or_prepare(index);
    }

    /*!
     * \brief Add a digit to the lowest end.
     *
     * \param[in] digit Digit.
     */
    void push_to_lowest(digit_type digit) { digits_.push_front(digit); }

    /*!
     * \brief Add a digit to the highest end.
     *
     * \param[in] digit Digit.
     */
    void push_to_highest(digit_type digit) { digits_.push_back(digit); }

    /*!
     * \brief Move digits.
     *
     * \param[in] offset Offset from the current places of digits.
     */
    void move_digits(index_type offset) { digits_.move_position(offset); }

    /*!
     * \brief Change the range of digits.
     *
     * \param[in] lowest Index of lowest digits.
     * \param[in] highest Index of the highest digit.
     */
    void resize(index_type lowest, index_type highest) {
        digits_.resize(lowest, highest);
    }

private:
    //! Digits.
    util::bidirectional_vector<digit_type> digits_;
};

}  // namespace num_collect::numbers
