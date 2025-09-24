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

#include <concepts>
#include <cstdint>

namespace num_collect::util {

/*!
 * \brief Convert an integer from binary to Gray code \cite Warren2013.
 *
 * \tparam T Type of the integer.
 * \param[in] binary Binary.
 * \return Gray code.
 */
template <typename T>
    requires(std::unsigned_integral<T> && sizeof(T) == sizeof(std::uint32_t))
[[nodiscard]] constexpr auto binary_to_gray_code(T binary) -> T {
    return binary ^ (binary >> 1U);
}

/*!
 * \brief Convert an integer from binary to Gray code \cite Warren2013.
 *
 * \tparam T Type of the integer.
 * \param[in] binary Binary.
 * \return Gray code.
 */
template <typename T>
    requires(std::unsigned_integral<T> && sizeof(T) == sizeof(std::uint64_t))
[[nodiscard]] constexpr auto binary_to_gray_code(T binary) -> T {
    return binary ^ (binary >> 1U);
}

/*!
 * \brief Convert an integer from Gray code to binary \cite Warren2013.
 *
 * \tparam T Type of the integer.
 * \param[in] gray_code Gray code.
 * \return Binary.
 */
template <typename T>
    requires(std::unsigned_integral<T> && sizeof(T) == sizeof(std::uint32_t))
[[nodiscard]] constexpr auto gray_code_to_binary(T gray_code) -> T {
    T binary = gray_code;
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
 * \tparam T Type of the integer.
 * \param[in] gray_code Gray code.
 * \return Binary.
 */
template <typename T>
    requires(std::unsigned_integral<T> && sizeof(T) == sizeof(std::uint64_t))
[[nodiscard]] constexpr auto gray_code_to_binary(T gray_code) -> T {
    T binary = gray_code;
    binary ^= binary >> 1U;
    binary ^= binary >> 2U;
    binary ^= binary >> 4U;
    binary ^= binary >> 8U;   // NOLINT(*-magic-numbers)
    binary ^= binary >> 16U;  // NOLINT(*-magic-numbers)
    binary ^= binary >> 32U;  // NOLINT(*-magic-numbers)
    return binary;
}

}  // namespace num_collect::util
