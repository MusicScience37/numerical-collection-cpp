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
 * \brief Definition of cbrt function.
 */
#pragma once

#include <cmath>

#include "num_collect/functions/root.h"

namespace num_collect::functions {

/*!
 * \brief Calculate cubic root \f$ \sqrt[3]{x} \f$.
 *
 * This function calculates similar values as
 * [cbrt](https://en.cppreference.com/w/cpp/numeric/math/cbrt) function in C++
 * standard library in constexpr.
 *
 * \tparam T Value type.
 * \param[in] x Value to calculate cubic root of.
 * \return Cubic root.
 */
template <typename T>
constexpr auto cbrt(T x) {
    if consteval {
        return root(x, 3);
    } else {
        return std::cbrt(x);
    }
}

}  // namespace num_collect::functions
