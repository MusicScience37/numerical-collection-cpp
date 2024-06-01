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
 * \brief Definition of least_common_multiple function.
 */
#pragma once

#include "num_collect/base/concepts/integral.h"  // IWYU pragma: keep
#include "num_collect/util/greatest_common_divisor.h"
#include "num_collect/util/multiply_safely.h"

namespace num_collect::util {

/*!
 * \brief Calculate the least common multiple.
 *
 * \tparam T Type of integers.
 * \param[in] a An integer.
 * \param[in] b An integer.
 * \return Least common multiple of the integers.
 */
template <base::concepts::integral T>
[[nodiscard]] constexpr auto least_common_multiple(T a, T b) -> T {
    return multiply_safely(a / greatest_common_divisor(a, b), b);
}

}  // namespace num_collect::util
