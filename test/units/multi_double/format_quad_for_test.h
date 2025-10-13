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
 * \brief Definition of format_quad_for_test function.
 */
#pragma once

#include <catch2/catch_tostring.hpp>
#include <fmt/format.h>

#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_io.h"  // IWYU pragma: keep

/*!
 * \brief Format a quad number for test.
 *
 * \param[in] value Value to format.
 * \return Formatted string.
 */
inline auto format_quad_for_test(const num_collect::multi_double::quad& value)
    -> std::string {
    return fmt::format(
        "quad({:.13a}, {:.13a}) ({})", value.high(), value.low(), value);
}

namespace Catch {

/*!
 * \brief Specialization of Catch::StringMaker for
 * num_collect::multi_double::quad class.
 */
template <>
struct StringMaker<num_collect::multi_double::quad> {
    /*!
     * \brief Convert a quad number to a string.
     *
     * \param[in] value Value to convert.
     * \return Converted string.
     */
    static auto convert(const num_collect::multi_double::quad& value)
        -> std::string {
        return format_quad_for_test(value);
    }
};

}  // namespace Catch
