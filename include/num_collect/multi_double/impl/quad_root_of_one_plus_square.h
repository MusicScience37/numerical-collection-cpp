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
 * \brief Definition of root_of_one_plus_square function.
 */
#pragma once

#include "num_collect/multi_double/impl/quad_sqrt_impl.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate \f$ \sqrt{1 + x^2} \f$.
 *
 * \param[in] x Input value.
 * \return Result.
 */
inline auto root_of_one_plus_square(quad x) noexcept -> quad {
    return sqrt_impl(quad(1.0) + x * x);
}

}  // namespace num_collect::multi_double::impl
