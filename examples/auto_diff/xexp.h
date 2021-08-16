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
 * \brief Definition of xexp function.
 */
#pragma once

#include <cmath>

/*!
 * \brief Calculate \f$ x \exp{x} \f$.
 *
 * \tparam T Type of argument.
 * \param[in] x Argument.
 * \return Result.
 */
template <typename T>
[[nodiscard]] inline auto xexp(const T& x) -> T {
    using std::exp;
    return x * exp(x);
}
