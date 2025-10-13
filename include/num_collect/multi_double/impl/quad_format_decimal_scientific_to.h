/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of format_decimal_scientific_to function for quad class.
 */
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>

#include <fmt/base.h>

#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_math.h"
#include "num_collect/util/assert.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Format a quad number in decimal scientific notation.
 *
 * \tparam OutputIterator Type of output iterator.
 * \param[out] out Output iterator to write characters.
 * \param[in] value Value to format.
 * \param[in] precision Number of digits after the decimal point.
 * \return Output iterator after writing characters.
 */
template <std::output_iterator<char> OutputIterator>
auto format_decimal_scientific_to(OutputIterator out, quad value,
    int precision = 30) -> OutputIterator  // NOLINT(*-magic-numbers)
{
    constexpr int double_precision = 14;
    if (precision <= double_precision || !std::isfinite(value.high())) {
        return fmt::format_to(out, "{:.{}e}", value.high(), precision);
    }

    constexpr int max_precision = 35;
    precision = std::min(precision, max_precision);

    if (value.high() == 0.0) {
        return fmt::format_to(out, "0.{0:0<{1}}e+00", "", precision);
    }
    if (value.high() < 0.0) {
        value = -value;
        *out = '-';
        ++out;
    }

    int exponent = static_cast<int>(std::floor(std::log10(value.high())));

    constexpr std::size_t digits_buffer_size =
        static_cast<std::size_t>(max_precision) + 3;
    std::array<char, digits_buffer_size> digits_buffer{};
    digits_buffer.fill(static_cast<char>(0));

    quad remaining =
        value / pow(quad(10.0), exponent);  // NOLINT(*-magic-numbers)
    // NOLINTNEXTLINE(*-sign-comparison)
    for (std::size_t i = 2; i < static_cast<std::size_t>(precision + 3); ++i) {
        double digit_double = std::floor(remaining.high());
        digits_buffer[i] = static_cast<char>(static_cast<int>(digit_double));
        remaining -= digit_double;
        remaining *= 10.0;  // NOLINT(*-magic-numbers)
    }
    // NOLINTNEXTLINE(*-sign-comparison)
    for (std::size_t i = static_cast<std::size_t>(precision + 2); i > 0; --i) {
        // NOLINTNEXTLINE(bugprone-signed-char-misuse,cert-str34-c): false positive
        const int current_digit = static_cast<int>(digits_buffer[i]);
        if (current_digit < 0 ||
            current_digit > 9) {                 // NOLINT(*-magic-numbers)
            int carry = current_digit / 10;      // NOLINT(*-magic-numbers)
            int new_digit = current_digit % 10;  // NOLINT(*-magic-numbers)
            if (new_digit < 0) {
                new_digit += 10;  // NOLINT(*-magic-numbers)
                --carry;
            }
            digits_buffer[i] = static_cast<char>(new_digit);
            digits_buffer[i - 1] = static_cast<char>(
                static_cast<int>(digits_buffer[i - 1]) + carry);
        }
    }
    const std::ptrdiff_t first_non_zero_index =
        std::ranges::find_if(digits_buffer, [](char c) { return c != 0; }) -
        digits_buffer.begin();
    NUM_COLLECT_ASSERT(first_non_zero_index + 1 + precision <=
        static_cast<std::ptrdiff_t>(digits_buffer_size));
    exponent -= static_cast<int>(first_non_zero_index) - 2;

    *out = static_cast<char>(
        static_cast<int>(
            digits_buffer[static_cast<std::size_t>(first_non_zero_index)]) +
        static_cast<int>('0'));
    ++out;
    if (precision > 0) {
        *out = '.';
        ++out;
        for (std::ptrdiff_t i = first_non_zero_index + 1;
            i < first_non_zero_index + 1 + precision; ++i) {
            *out = static_cast<char>(
                static_cast<int>(digits_buffer[static_cast<std::size_t>(i)]) +
                static_cast<int>('0'));
            ++out;
        }
    }
    return fmt::format_to(out, "e{:+03d}", exponent);
}

}  // namespace num_collect::multi_double::impl
