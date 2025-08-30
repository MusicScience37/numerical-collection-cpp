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
 * \brief Definition of sech_rbf class.
 */
#pragma once

#include <cmath>
#include <limits>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of sech RBF \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class sech_rbf {
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
        using std::cosh;
        return static_cast<scalar_type>(1) / cosh(distance_rate);
    }
};

/*!
 * \brief Class of differentiated sech RBF.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class differentiated_sech_rbf {
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
        using std::cosh;
        using std::sinh;
        if (distance_rate < std::numeric_limits<scalar_type>::epsilon()) {
            return static_cast<scalar_type>(1);
        }
        return sinh(distance_rate) /
            (distance_rate * cosh(distance_rate) * cosh(distance_rate));
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::sech_rbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct differentiated<sech_rbf<Scalar>> {
    //! Type of the differentiated RBF.
    using type = differentiated_sech_rbf<Scalar>;
};

}  // namespace num_collect::rbf::rbfs
