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
 * \brief Definition of log1m_maclaurin function.
 */
#pragma once

#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep

namespace num_collect::constants::impl {

/*!
 * \brief Calculate logarithm of 1 - x.
 *
 * This calculates the following series:
 * \f[
 *     log(1 - x) = -\sum_{k=1}^\infty \frac{x^k}{k}
 * \f]
 *
 * \note This function is suitable for small positive numbers.
 *
 * \tparam T Number type.
 * \param[in] x Number.
 * \return logarithm of 1 - x.
 */
template <typename T>
constexpr auto log1m_maclaurin(T x) -> T {
    T sum = zero<T>;
    T prod = one<T>;
    constexpr int max_loops = 1000;
    for (int i = 1; i <= max_loops; ++i) {
        T prev_sum = sum;
        prod *= x;
        sum += prod / static_cast<T>(i);
        if (sum == prev_sum) {
            break;
        }
    }
    return -sum;
}

}  // namespace num_collect::constants::impl
