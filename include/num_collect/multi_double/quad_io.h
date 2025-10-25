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
 * \brief Definition of IO functions of quad class.
 */
#pragma once

#include <cctype>
#include <string_view>

#include <fmt/base.h>

#include "num_collect/multi_double/impl/quad_format_decimal_scientific_to.h"
#include "num_collect/multi_double/quad.h"

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for num_collect::multi_double::quad.
 */
template <>
struct formatter<num_collect::multi_double::quad> {
    /*!
     * \brief Parse a format specification.
     *
     * \param[in] context Context.
     * \return Iterator after the parsed part.
     */
    constexpr auto parse(format_parse_context& context)
        -> format_parse_context::iterator {
        // NOLINTNEXTLINE(readability-qualified-auto)
        auto iter = context.begin();

        if (iter == context.end() || *iter == '}') {
            return iter;
        }
        if (!is_digit(*iter) && *iter != '.' && *iter != 'e') {
            throw format_error("Invalid format specification.");
        }

        if (is_digit(*iter)) {
            // Width.
            width_ = 0;
            while (iter != context.end() && is_digit(*iter)) {
                width_ = width_ * 10 + (static_cast<int>(*iter) - '0');
                ++iter;
            }
        }

        if (iter == context.end() || *iter == '}') {
            return iter;
        }
        if (*iter != '.' && *iter != 'e') {
            throw format_error("Invalid format specification.");
        }

        if (*iter == '.') {
            // Precision.
            ++iter;
            if (iter == context.end() && !is_digit(*iter)) {
                throw format_error("Expected precision.");
            }
            precision_ = 0;
            while (iter != context.end() && is_digit(*iter)) {
                precision_ = precision_ * 10 + (static_cast<int>(*iter) - '0');
                ++iter;
            }
        }

        if (iter == context.end() || *iter == '}') {
            return iter;
        }
        if (*iter != 'e') {
            throw format_error("Invalid format specification.");
        }

        if (*iter == 'e') {
            // Scientific notation.
            ++iter;
        }

        if (iter == context.end() || *iter == '}') {
            return iter;
        }
        throw format_error("Invalid format specification.");
    }

    /*!
     * \brief Format a value.
     *
     * \param[in] value Value.
     * \param[in] context Context.
     * \return Iterator after the formatted part.
     */
    auto format(const num_collect::multi_double::quad& value,
        format_context& context) const -> format_context::iterator {
        if (width_ == 0) {
            return num_collect::multi_double::impl::
                format_decimal_scientific_to(context.out(), value, precision_);
        }
        constexpr std::size_t buffer_size = 45;
        std::array<char, buffer_size> buffer{};
        // NOLINTNEXTLINE(readability-qualified-auto)
        const auto end =
            num_collect::multi_double::impl::format_decimal_scientific_to(
                buffer.data(), value, precision_);
        return fmt::format_to(context.out(), "{: >{}}",
            std::string_view(buffer.data(), end), width_);
    }

private:
    /*!
     * \brief Check whether a character is a digit.
     *
     * \param[in] c Character.
     * \retval true Character is a digit.
     * \retval false Character is not a digit.
     *
     * \note This function is constexpr version of std::isdigit.
     */
    static constexpr auto is_digit(const char c) noexcept -> bool {
        return '0' <= c && c <= '9';
    }

    //! Width.
    int width_{0};

    //! Precision.
    int precision_{30};
};

}  // namespace fmt
