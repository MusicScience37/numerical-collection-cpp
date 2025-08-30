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
 * \brief Definition of inverse_quadratic_rbf class.
 */
#pragma once

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of inverse quadratic RBF \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class inverse_quadratic_rbf {
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
        return static_cast<scalar_type>(1) /
            (static_cast<scalar_type>(1) + distance_rate * distance_rate);
    }
};

/*!
 * \brief Class of differentiated inverse quadric RBF.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class differentiated_inverse_quadratic_rbf {
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
        const scalar_type factor =
            static_cast<scalar_type>(1) + distance_rate * distance_rate;
        return static_cast<scalar_type>(2) / (factor * factor);
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::inverse_quadratic_rbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct differentiated<inverse_quadratic_rbf<Scalar>> {
    //! Type of the differentiated RBF.
    using type = differentiated_inverse_quadratic_rbf<Scalar>;
};

}  // namespace num_collect::rbf::rbfs
