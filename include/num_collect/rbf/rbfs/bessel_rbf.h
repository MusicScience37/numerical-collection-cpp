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

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/functions/bessel.h"

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

        static constexpr auto bessel_order =
            static_cast<scalar_type>(Dimension) / static_cast<scalar_type>(2) -
            static_cast<scalar_type>(1);

        static constexpr auto threshold = static_cast<scalar_type>(1e-5);
        if (abs(distance_rate) < threshold) {
            // TODO Better approximation.
            return functions::cyl_bessel_j(bessel_order, threshold) /
                pow(threshold, bessel_order);
        }

        return functions::cyl_bessel_j(bessel_order, distance_rate) /
            pow(distance_rate, bessel_order);
    }
};

}  // namespace num_collect::rbf::rbfs
