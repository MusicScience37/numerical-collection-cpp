/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Declarations of functions to calculate Bessel functions.
 */
#pragma once

#include "num_collect/impl/num_collect_export.h"

namespace num_collect::functions {

/*!
 * \brief Calculate the cylindrical Bessel function of the first kind, \f$
 * J_{\nu}(x) \f$.
 *
 * \param[in] nu Order of the Bessel function.
 * \param[in] x Argument of the Bessel function.
 * \return Value of the Bessel function.
 *
 * \note This function is an implementation of `std::cyl_bessel_j` function in
 * C++ standard library. This function is implemented because some environments
 * do not support `std::cyl_bessel_j` function.
 * This function is implemented as a wrapper of Boost.Math library.
 */
NUM_COLLECT_EXPORT auto cyl_bessel_j(float nu, float x) -> float;

/*!
 * \brief Calculate the cylindrical Bessel function of the first kind, \f$
 * J_{\nu}(x) \f$.
 *
 * \param[in] nu Order of the Bessel function.
 * \param[in] x Argument of the Bessel function.
 * \return Value of the Bessel function.
 *
 * \note This function is an implementation of `std::cyl_bessel_j` function in
 * C++ standard library. This function is implemented because some environments
 * do not support `std::cyl_bessel_j` function.
 * This function is implemented as a wrapper of Boost.Math library.
 */
NUM_COLLECT_EXPORT auto cyl_bessel_j(double nu, double x) -> double;

/*!
 * \brief Calculate the cylindrical Bessel function of the first kind, \f$
 * J_{\nu}(x) \f$.
 *
 * \param[in] nu Order of the Bessel function.
 * \param[in] x Argument of the Bessel function.
 * \return Value of the Bessel function.
 *
 * \note This function is an implementation of `std::cyl_bessel_j` function in
 * C++ standard library. This function is implemented because some environments
 * do not support `std::cyl_bessel_j` function.
 * This function is implemented as a wrapper of Boost.Math library.
 */
NUM_COLLECT_EXPORT auto cyl_bessel_j(long double nu, long double x)
    -> long double;

// TODO Other Bessel functions.

}  // namespace num_collect::functions
