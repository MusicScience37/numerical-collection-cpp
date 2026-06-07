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
 * \brief Definition of bessel_rbf class.
 */
#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/functions/bessel.h"
#include "num_collect/functions/gamma.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of Bessel RBF \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Dimension Dimension of the space of the RBF.
 *
 * This RBF is positive definite if the dimension of the space is less than or
 * equal to the parameter `Dimension` if `Dimension` is greater than or equal
 * to 2.
 *
 * This RBF is defined as follows:
 *
 * \f[
 * \phi(r) \equiv \frac{J_{d/2-1}(r)}{r^{d/2-1}}
 * \f]
 *
 * where \f$d\f$ is the parameter `Dimension` and \f$J_{\nu}\f$ is the Bessel
 * function of the first kind of order \f$\nu\f$.
 *
 * The singularity at \f$r=0\f$ can be removed by using the expansion of the
 * Bessel function:
 *
 * \f[
 * \frac{J_{\nu}(r)}{r^{\nu}} =
 * \left(\frac{1}{2}\right)^{\nu}
 * \sum_{m = 0}^{\infty} \frac{(-1)^m}{m! \Gamma(m + \nu + 1)}
 * \left(\frac{r}{2}\right)^{2m}
 * \f]
 */
template <base::concepts::real_scalar Scalar, index_type Dimension>
    requires(Dimension >= 1)
class bessel_rbf {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Whether this RBF is positive definite.
    static constexpr bool is_positive_definite = Dimension >= 2;

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

        static constexpr auto bessel_order =
            static_cast<scalar_type>(Dimension) / static_cast<scalar_type>(2) -
            static_cast<scalar_type>(1);

        static constexpr auto threshold = static_cast<scalar_type>(1e-2);
        if (distance_rate < threshold) {
            // Use the expansion of the Bessel function.
            const auto half_distance_rate =
                static_cast<scalar_type>(0.5) * distance_rate;
            const auto half_distance_rate_squared =
                half_distance_rate * half_distance_rate;
            scalar_type term =
                pow(static_cast<scalar_type>(0.5), bessel_order) /
                functions::gamma(bessel_order + static_cast<scalar_type>(1));
            scalar_type sum = term;
            // The term decreases at least the rate of 1e-4, so the number of
            // terms can be limited by the following.
            constexpr index_type max_m =
                std::numeric_limits<scalar_type>::digits10 / 4 + 1;
            for (index_type m = 1; m <= max_m; ++m) {
                term *= -half_distance_rate_squared /
                    (static_cast<scalar_type>(m) *
                        (static_cast<scalar_type>(m) + bessel_order));
                sum += term;
            }
            return sum;
        }

        return functions::cyl_bessel_j(bessel_order, distance_rate) /
            pow(distance_rate, bessel_order);
    }
};

}  // namespace num_collect::rbf::rbfs
