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
 * \brief Definition of polyharmonic_spline_rbf class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of RBF of polyharmonic splines \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
class polyharmonic_spline_rbf;

/*!
 * \brief Class of RBF of polyharmonic splines \cite Fornberg2015 for odd
 * degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
    requires(Degree >= 1 && (Degree % 2 == 1))
class polyharmonic_spline_rbf<Scalar, Degree> {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::pow;
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));
        return static_cast<scalar_type>(pow(distance_rate, Degree));
    }
};

/*!
 * \brief Class of RBF of polyharmonic splines \cite Fornberg2015 for even
 * degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
    requires(Degree >= 1 && (Degree % 2 == 0))
class polyharmonic_spline_rbf<Scalar, Degree> {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::log;
        using std::pow;
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));
        if (distance_rate == static_cast<scalar_type>(0)) {
            return static_cast<scalar_type>(0);
        }
        return static_cast<scalar_type>(pow(distance_rate, Degree)) *
            log(distance_rate);
    }
};

}  // namespace num_collect::rbf::rbfs
