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
 * \brief Definition of matern_rbf class.
 */
#pragma once

#include <cmath>
#include <concepts>
#include <cstdlib>

#include "num_collect/functions/bessel.h"
#include "num_collect/functions/gamma.h"
#include "num_collect/functions/pow.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of Matérn RBF \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars. Current implementation supports only `float`
 * and `double`.
 * \tparam Order Order of the Bessel function used in this RBF.
 */
template <std::floating_point Scalar, Scalar Order>
    requires(Order > static_cast<Scalar>(0))
class matern_rbf {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Whether this RBF is positive definite.
    static constexpr bool is_positive_definite = true;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::abs;
        using std::pow;

        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));

        // TODO Make this constexpr.
        static const auto coeff =
            functions::pow(static_cast<scalar_type>(2), 1 - Order) /
            functions::gamma(Order);

        static constexpr auto threshold = static_cast<scalar_type>(1e-5);
        if (distance_rate < threshold) {
            // Approximate the value near zero by the value at the small
            // threshold.
            return coeff * pow(threshold, Order) *
                functions::cyl_bessel_k(Order, threshold);
        }
        return coeff * pow(distance_rate, Order) *
            functions::cyl_bessel_k(Order, distance_rate);
    }
};

}  // namespace num_collect::rbf::rbfs
