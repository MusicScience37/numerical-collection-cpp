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
 * \brief Definition of gamma_lanczos class.
 */
#pragma once

#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/constants/half.h"  // IWYU pragma: keep

namespace num_collect::functions::impl {

/*!
 * \brief Class to calculate gamma function using approximation derived by
 * Lanczos \cite Press2007.
 */
template <base::concepts::real_scalar Real>
class gamma_lanczos {
public:
    //! Number of coefficients.
    static constexpr std::size_t num_coeffs = 14;

    //! Coefficients.
    static constexpr std::array<Real, num_coeffs> coeffs{
        static_cast<Real>(57.1562356658629235),
        static_cast<Real>(-59.5979603554754912),
        static_cast<Real>(14.1360979747417471),
        static_cast<Real>(-0.491913816097620199),
        static_cast<Real>(0.339946499848118887e-4),
        static_cast<Real>(0.465236289270485756e-4),
        static_cast<Real>(-0.983744753048795646e-4),
        static_cast<Real>(0.158088703224912494e-3),
        static_cast<Real>(-0.210264441724104883e-3),
        static_cast<Real>(0.217439618115212643e-3),
        static_cast<Real>(-0.164318106536762890e-3),
        static_cast<Real>(0.844182239838527433e-4),
        static_cast<Real>(-0.261908384015814087e-4),
        static_cast<Real>(0.368991826295316234e-5)};

    //! Constant term.
    static constexpr auto constant = static_cast<Real>(0.999999999999997092);

    //! Coefficient for the series.
    static constexpr auto series_coeff = static_cast<Real>(2.5066282746310005);

    //! Rational for parameter.
    static constexpr auto rational =
        static_cast<Real>(671) / static_cast<Real>(128);

    /*!
     * \brief Calculate a value of gamma function.
     *
     * \tparam T Type of the argument.
     * \param[in] x Argument.
     * \return Value of gamma function.
     */
    template <typename T>
        requires(std::is_same_v<T, Real> ||
                    std::is_same_v<T, std::complex<Real>>)
    [[nodiscard]] static constexpr auto gamma(T x) -> T {
        const T offset_x = x + rational;
        T series_sum = constant;
        for (std::size_t i = 0; i < num_coeffs; ++i) {
            series_sum += coeffs[i] / (x + static_cast<Real>(i + 1U));
        }
        using std::exp;
        using std::log;
        using std::pow;
        return pow(offset_x, x + constants::half<Real>) * exp(-offset_x) *
            series_coeff * series_sum / x;
    }

    /*!
     * \brief Calculate a natural logarithm of a value of gamma function.
     *
     * \param[in] x Argument.
     * \return Natural logarithm of the value of gamma function.
     */
    [[nodiscard]] static constexpr auto log_gamma(Real x) -> Real {
        const Real offset_x = x + rational;
        Real series_sum = constant;
        for (std::size_t i = 0; i < num_coeffs; ++i) {
            series_sum += coeffs[i] / (x + static_cast<Real>(i + 1U));
        }
        using std::log;
        return (x + constants::half<Real>)*log(offset_x) - offset_x +
            log(series_coeff * series_sum / x);
    }
};

}  // namespace num_collect::functions::impl
