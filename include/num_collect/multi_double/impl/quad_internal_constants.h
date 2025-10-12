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
 * \brief Definitions of internal constants of quad class.
 */
#pragma once

#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double::impl {

/*!
 * \brief \f$ \log{2} \f$ as quad value.
 */
constexpr auto log2_quad =  // cspell: disable-next-line
    quad(0x1.62e42fefa39efp-1, 0x1.abc9e3b39803fp-56);

/*!
 * \brief \f$ 1 / \log{2} \f$ as quad value.
 */
constexpr auto log2_inv_quad =  // cspell: disable-next-line
    quad(0x1.71547652b82fep+0, 0x1.777d0ffda0d20p-56);

/*!
 * \brief \f$ \log{10} \f$ as quad value.
 */
constexpr auto log10_quad =  // cspell: disable-next-line
    quad(0x1.26bb1bbb55516p+1, -0x1.f48ad494ea3e9p-53);

/*!
 * \brief \f$ 1 / \log{10} \f$ as quad value.
 */
constexpr auto log10_inv_quad =  // cspell: disable-next-line
    quad(0x1.bcb7b1526e50ep-2, 0x1.95355baaafad0p-57);

/*!
 * \brief Maximum absolute value for input of expm1_maclaurin_series,
 * exp_maclaurin_series functions.
 */
constexpr auto exp_maclaurin_limit_quad =  // cspell: disable-next-line
    quad(0x1.64840e1719f80p-10, -0x1.cd5f99c38b04cp-64);

}  // namespace num_collect::multi_double::impl
