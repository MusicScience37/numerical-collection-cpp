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
 * \brief Implementations of functions to calculate Bessel functions.
 */
#include "num_collect/functions/bessel.h"

#include <boost/math/special_functions/bessel.hpp>

namespace num_collect::functions {

auto cyl_bessel_j(float nu, float x) -> float {
    return boost::math::cyl_bessel_j(nu, x);
}

auto cyl_bessel_j(double nu, double x) -> double {
    return boost::math::cyl_bessel_j(nu, x);
}

auto cyl_bessel_j(long double nu, long double x) -> long double {
    return boost::math::cyl_bessel_j(nu, x);
}

auto cyl_neumann(float nu, float x) -> float {
    return boost::math::cyl_neumann(nu, x);
}

auto cyl_neumann(double nu, double x) -> double {
    return boost::math::cyl_neumann(nu, x);
}

auto cyl_neumann(long double nu, long double x) -> long double {
    return boost::math::cyl_neumann(nu, x);
}

auto cyl_bessel_i(float nu, float x) -> float {
    return boost::math::cyl_bessel_i(nu, x);
}

auto cyl_bessel_i(double nu, double x) -> double {
    return boost::math::cyl_bessel_i(nu, x);
}

auto cyl_bessel_i(long double nu, long double x) -> long double {
    return boost::math::cyl_bessel_i(nu, x);
}

auto cyl_bessel_k(float nu, float x) -> float {
    return boost::math::cyl_bessel_k(nu, x);
}

auto cyl_bessel_k(double nu, double x) -> double {
    return boost::math::cyl_bessel_k(nu, x);
}

auto cyl_bessel_k(long double nu, long double x) -> long double {
    return boost::math::cyl_bessel_k(nu, x);
}

}  // namespace num_collect::functions
