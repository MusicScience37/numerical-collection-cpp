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
 * \brief Definition of multiply_safely function.
 */
#pragma once

#include <concepts>

#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::util {

/*!
 * \brief Multiply two integers with check of overflow.
 *
 * \tparam T Type of integers.
 * \param[in] a An integer.
 * \param[in] b An integer.
 * \return Result of multiplication.
 */
template <std::integral T>
[[nodiscard]] constexpr auto multiply_safely(T a, T b) -> T {
    const T res = a * b;
    if (a != static_cast<T>(0) && res / a != b) {
        NUM_COLLECT_LOG_AND_THROW(
            algorithm_failure, "Overflow in multiplication.");
    }
    return res;
}

}  // namespace num_collect::util
