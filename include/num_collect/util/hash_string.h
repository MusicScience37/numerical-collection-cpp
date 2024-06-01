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
 * \brief Definition of hash_string function.
 */
#pragma once

#include <cstdint>
#include <string_view>

namespace num_collect::util {

/*!
 * \brief Calculate a hash number of a string.
 *
 * \note This function can calculate a hash number at compile-time.
 *
 * \param[in] str String.
 * \return Hash number.
 */
[[nodiscard]] constexpr auto hash_string(
    std::string_view str) noexcept -> std::uint32_t {
    constexpr std::uint32_t init_num = 0x5928ACD1U;
    std::uint32_t num = init_num;
    for (char c : str) {
        const auto added =
            static_cast<std::uint32_t>(static_cast<unsigned char>(c));
        constexpr std::uint32_t shift = 5U;
        num += (num << shift) + added;
    }
    return num;
}

}  // namespace num_collect::util
