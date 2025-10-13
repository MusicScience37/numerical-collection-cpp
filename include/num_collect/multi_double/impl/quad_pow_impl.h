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
 * \brief Definition of internal functions to calculate pow function of quad
 * class.
 */
#pragma once

#include "num_collect/multi_double/impl/quad_exp_impl.h"
#include "num_collect/multi_double/impl/quad_log_impl.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief Calculate the value of `base` raised to the power of `exponent`
 * using exp and log functions.
 *
 * \param[in] base Base value.
 * \param[in] exponent Exponent value.
 * \return Result.
 *
 * This function can handle only positive base values.
 */
inline auto pow_general_impl(quad base, quad exponent) noexcept -> quad {
    return exp_impl(log_impl(base) * exponent);
}

}  // namespace num_collect::multi_double::impl
