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
 * \brief Definition of basic operations in multi-double
 * calculations
 */
#pragma once

#include <tuple>

#ifdef __AVX2__
#include <emmintrin.h>
#include <immintrin.h>
#endif

#ifdef __FAST_MATH__
#warning "Use of -ffast-math is unsafe for multi_double module."
#endif

namespace num_collect::multi_double::impl {

/*!
 * \brief calculate sum of a and b, and error of the sum
 *        on the condition that absolute value of a is larger than one of b
 *
 * \param[in] a a number
 * \param[in] b a number
 * \return sum of a and b, and error of sum
 */
inline auto quick_two_sum(double a, double b) -> std::tuple<double, double> {
    const double s = a + b;
    const double e = b - (s - a);
    return {s, e};
}

/*!
 * \brief calculate sum of a and b, and error of the sum
 *
 * \param[in] a a number
 * \param[in] b a number
 * \return sum of a and b, and error of sum
 */
inline auto two_sum(double a, double b) -> std::tuple<double, double> {
    const double s = a + b;
    const double v = s - a;
    const double e = (a - (s - v)) + (b - v);
    return {s, e};
}

/*!
 * \brief split a number to higher bits and lower bits
 *
 * \param[in] a a number
 * \return higher bits and lower bits
 */
inline auto split(double a) -> std::tuple<double, double> {
    constexpr double coeff = 0x1.0p+27 + 1.0;
    const double t = coeff * a;
    const double a_h = t - (t - a);
    const double a_l = a - a_h;
    return {a_h, a_l};
}

/*!
 * \brief calculate product of a and b, and error of the product without FMA
 * instructions
 *
 * \param[in] a a number
 * \param[in] b a number
 * \return product of a and b, and error of product
 */
inline auto two_prod_no_fma(double a, double b) -> std::tuple<double, double> {
    const double p = a * b;
    const auto [a_h, a_l] = split(a);
    const auto [b_h, b_l] = split(b);
    const double e = ((a_h * b_h - p) + a_h * b_l + a_l * b_h) + a_l * b_l;
    return {p, e};
}

#ifdef __AVX2__

/*!
 * \brief calculate product of a and b, and error of the product with FMA
 * instructions
 *
 * \param[in] a a number
 * \param[in] b a number
 * \return product of a and b, and error of product
 */
inline auto two_prod_fma(double a, double b) -> std::tuple<double, double> {
    const double p = a * b;
    const __m128d a_mm = _mm_set_sd(a);
    const __m128d b_mm = _mm_set_sd(b);
    const __m128d p_mm = _mm_set_sd(p);
    const __m128d e_mm = _mm_fmsub_sd(a_mm, b_mm, p_mm);
    double e = 0.0;
    _mm_store_sd(&e, e_mm);
    return {p, e};
}

#endif

/*!
 * \brief calculate product of a and b, and error of the product
 *
 * \param[in] a a number
 * \param[in] b a number
 * \return product of a and b, and error of product
 */
inline auto two_prod(double a, double b) -> std::tuple<double, double> {
#ifdef __AVX2__
    return two_prod_fma(a, b);
#else
    return two_prod_no_fma(a, b);
#endif
}

}  // namespace num_collect::multi_double::impl
