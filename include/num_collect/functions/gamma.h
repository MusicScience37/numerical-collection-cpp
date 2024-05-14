/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of gamma function.
 */
#pragma once

#include <cmath>
#include <complex>

#include "num_collect/constants/pi.h"
#include "num_collect/functions/impl/gamma_lanczos.h"
namespace num_collect::functions {

/*!
 * \brief Calculate a value of gamma function.
 *
 * \param[in] x Argument.
 * \return Value.
 */
[[nodiscard]] inline constexpr auto gamma(float x) -> float {
    if (x < 1.0F) {
        const float pi_1mx = constants::pi<float> * (1.0F - x);
        return pi_1mx / std::sin(pi_1mx) /
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            impl::gamma_lanczos<float>::gamma(2.0F - x);
    }
    return impl::gamma_lanczos<float>::gamma(x);
}

/*!
 * \brief Calculate a value of gamma function.
 *
 * \param[in] x Argument.
 * \return Value.
 */
[[nodiscard]] inline constexpr auto gamma(double x) -> double {
    if (x < 1.0) {
        const double pi_1mx = constants::pi<double> * (1.0 - x);
        return pi_1mx / std::sin(pi_1mx) /
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            impl::gamma_lanczos<double>::gamma(2.0 - x);
    }
    return impl::gamma_lanczos<double>::gamma(x);
}

/*!
 * \brief Calculate a value of gamma function.
 *
 * \param[in] x Argument.
 * \return Value.
 */
[[nodiscard]] inline constexpr auto gamma(std::complex<float> x)
    -> std::complex<float> {
    if (x.real() < 1.0F) {
        const std::complex<float> pi_1mx = constants::pi<float> * (1.0F - x);
        return pi_1mx / std::sin(pi_1mx) /
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            impl::gamma_lanczos<float>::gamma(2.0F - x);
    }
    return impl::gamma_lanczos<float>::gamma(x);
}

/*!
 * \brief Calculate a value of gamma function.
 *
 * \param[in] x Argument.
 * \return Value.
 */
[[nodiscard]] inline constexpr auto gamma(std::complex<double> x)
    -> std::complex<double> {
    if (x.real() < 1.0) {
        const std::complex<double> pi_1mx = constants::pi<double> * (1.0 - x);
        return pi_1mx / std::sin(pi_1mx) /
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            impl::gamma_lanczos<double>::gamma(2.0 - x);
    }
    return impl::gamma_lanczos<double>::gamma(x);
}

/*!
 * \brief Calculate a natural logarithm of a value of gamma function.
 *
 * \param[in] x Argument.
 * \return Value.
 */
[[nodiscard]] inline constexpr auto log_gamma(float x) -> float {
    return impl::gamma_lanczos<float>::log_gamma(x);
}

/*!
 * \brief Calculate a natural logarithm of a value of gamma function.
 *
 * \param[in] x Argument.
 * \return Value.
 */
[[nodiscard]] inline constexpr auto log_gamma(double x) -> double {
    return impl::gamma_lanczos<double>::log_gamma(x);
}

}  // namespace num_collect::functions
