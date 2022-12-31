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
 * \brief Definition of is_log_tag_element_char function.
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>

namespace num_collect::logging::impl {

/*!
 * \brief Create a mask array for is_log_tag_element_char function.
 *
 * \return Mask array.
 */
[[nodiscard]] inline constexpr auto create_log_tag_element_char_mask() {
    constexpr std::size_t size = 256;
    std::array<bool, size> res{};
    std::fill(res.begin(), res.end(), false);

    constexpr auto num_begin = static_cast<char>(0x30);  // 0
    constexpr auto num_end = static_cast<char>(0x39);    // 9
    for (char c = num_begin; c <= num_end; ++c) {
        res[c] = true;
    }

    constexpr auto large_alpha_begin = static_cast<char>(0x41);  // A
    constexpr auto large_alpha_end = static_cast<char>(0x5A);    // Z
    for (char c = large_alpha_begin; c <= large_alpha_end; ++c) {
        res[c] = true;
    }

    constexpr auto small_alpha_begin = static_cast<char>(0x61);  // a
    constexpr auto small_alpha_end = static_cast<char>(0x7A);    // z
    for (char c = small_alpha_begin; c <= small_alpha_end; ++c) {
        res[c] = true;
    }

    constexpr auto underscore = 0x5F;  // _
    res[underscore] = true;

    return res;
}

/*!
 * \brief Check whether the given character is permitted in elements of log
 * tags.
 *
 * \param[in] c Character to check.
 * \retval true Permitted.
 * \retval false Not permitted.
 */
[[nodiscard]] inline auto is_log_tag_element_char(char c) noexcept -> bool {
    static constexpr auto mask = create_log_tag_element_char_mask();
    return mask[static_cast<std::uint8_t>(c)];
}

}  // namespace num_collect::logging::impl
