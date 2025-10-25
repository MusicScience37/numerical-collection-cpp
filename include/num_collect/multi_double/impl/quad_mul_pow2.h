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
 * \brief Definition of mul_pow2 function for quad class.
 */
#pragma once

#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Multiply by power of two.
 *
 * \param[in] q `quad` value.
 * \param[in] pow2 Power of two.
 * \return Result.
 */
constexpr auto mul_pow2(quad q, double pow2) noexcept -> quad {
    return quad(q.high() * pow2, q.low() * pow2);
}

}  // namespace num_collect::multi_double::impl
