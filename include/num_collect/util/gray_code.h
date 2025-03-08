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
 * \brief Definition of functions for gray code.
 */
#pragma once

#include <cstdint>

namespace num_collect::util {

/*!
 * \brief Convert an integer from binary to Gray code \cite Warren2013.
 *
 * \param[in] binary Binary.
 * \return Gray code.
 */
[[nodiscard]] constexpr auto binary_to_gray_code(std::uint32_t binary)
    -> std::uint32_t {
    return binary ^ (binary >> 1U);
}

/*!
 * \brief Convert an integer from binary to Gray code \cite Warren2013.
 *
 * \param[in] binary Binary.
 * \return Gray code.
 */
[[nodiscard]] constexpr auto binary_to_gray_code(std::uint64_t binary)
    -> std::uint64_t {
    return binary ^ (binary >> 1U);
}

/*!
 * \brief Convert an integer from Gray code to binary \cite Warren2013.
 *
 * \param[in] gray_code Gray code.
 * \return Binary.
 */
[[nodiscard]] constexpr auto gray_code_to_binary(std::uint32_t gray_code)
    -> std::uint32_t {
    std::uint32_t binary = gray_code;
    binary ^= binary >> 1U;
    binary ^= binary >> 2U;
    binary ^= binary >> 4U;
    binary ^= binary >> 8U;   // NOLINT(*-magic-numbers)
    binary ^= binary >> 16U;  // NOLINT(*-magic-numbers)
    return binary;
}

/*!
 * \brief Convert an integer from Gray code to binary \cite Warren2013.
 *
 * \param[in] gray_code Gray code.
 * \return Binary.
 */
[[nodiscard]] constexpr auto gray_code_to_binary(std::uint64_t gray_code)
    -> std::uint64_t {
    std::uint64_t binary = gray_code;
    binary ^= binary >> 1U;
    binary ^= binary >> 2U;
    binary ^= binary >> 4U;
    binary ^= binary >> 8U;   // NOLINT(*-magic-numbers)
    binary ^= binary >> 16U;  // NOLINT(*-magic-numbers)
    binary ^= binary >> 32U;  // NOLINT(*-magic-numbers)
    return binary;
}

}  // namespace num_collect::util
